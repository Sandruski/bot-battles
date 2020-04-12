#include "WeaponSystemClient.h"

#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystemClient::WeaponSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (!clientComponent.IsLocalEntity(entity)) {
            continue;
        }

        if (clientComponent.m_isLastInputWeaponPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            const InputComponent& inputComponent = input.GetInputComponent();

            if (inputComponent.m_isShooting) {
                std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                glm::vec2 position = transformComponent.lock()->m_position;
                glm::vec2 rotation = transformComponent.lock()->GetRotation();
                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_currentResolution.x, windowComponent.m_currentResolution.y));
                std::pair<Entity, std::weak_ptr<ColliderComponent>> object;
                glm::vec2 intersection;
                const bool hasIntersected = Raycast(position, rotation, maxLength, object, intersection);
                if (hasIntersected) {
                    Entity hitEntity = object.first;

                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(hitEntity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destination = intersection;
                    }

                    std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(hitEntity);
                    if (!hitEntityHealthComponent.expired()) {
                        weaponComponent.lock()->m_hasHit = true;
                    } else {
                        weaponComponent.lock()->m_hasHit = false;
                    }
                } else {
                    weaponComponent.lock()->m_destination = position + rotation * maxLength;
                    weaponComponent.lock()->m_hasHit = false;
                }

                if (clientComponent.m_isClientPrediction) {
                    Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
                    transformComponent.lock()->m_inputTransformBuffer.Add(transform);
                }
            }

            clientComponent.m_isLastInputWeaponPending = false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Render()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec4 color = White;
        if (clientComponent.IsLocalEntity(entity)) {
            switch (clientComponent.m_playerID) {
            case 0: {
                color = Red;
                break;
            }
            case 1: {
                color = Blue;
                break;
            }
            default: {
                break;
            }
            }
        } else {
            switch (clientComponent.m_playerID) {
            case 0: {
                color = Blue;
                break;
            }
            case 1: {
                color = Red;
                break;
            }
            default: {
                break;
            }
            }
        }

        glm::mat4 model = glm::mat4(1.0f);

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        // From
        glm::vec2 from = weaponComponent.lock()->m_origin;
        from.x *= proportion.x;
        from.y *= proportion.y;
        from.y *= -1.0f;
        vertices[0].m_position = from;
        // To
        glm::vec2 to = weaponComponent.lock()->m_destination;
        to.x *= proportion.x;
        to.y *= proportion.y;
        to.y *= -1.0f;
        vertices[2].m_position = to;
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 colorLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(weaponComponent.lock()->m_hasHit ? Yellow : color));

        U32 pctLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "pct");
        glUniform1f(pctLoc, 1.0f);

        glBindVertexArray(rendererComponent.m_meshResource.lock()->GetVAO());
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    return true;
}
}
