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
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        if (clientComponent.m_isLastInputWeaponPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            const InputComponent& inputComponent = input.GetInputComponent();

            if (inputComponent.m_isShooting) {
                std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
                weaponComponent.lock()->m_positions.clear();

                LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
                const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

                for (const auto& pair : newtorkIDToEntity) {
                    Entity remoteEntity = pair.second;
                    if (entity == remoteEntity) {
                        continue;
                    }

                    std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
                    if (colliderComponent.expired()) {
                        continue;
                    }

                    weaponComponent.lock()->m_positions.emplace_back(colliderComponent.lock()->m_position);
                }

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                glm::vec2 position = transformComponent.lock()->m_position;
                glm::vec2 rotation = transformComponent.lock()->GetRotation();
                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                glm::uvec2 resolution = windowComponent.GetResolution();
                F32 maxLength = static_cast<F32>(std::max(resolution.x, resolution.y));
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
    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    glm::uvec2 resolution = windowComponent.GetResolution();

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = transformComponent.lock()->GetDebugPosition();
        position.x = resolution.x / 2.0f;
        position.y = resolution.y / 2.0f;
        model = glm::translate(model, position);

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        // From
        vertices[0].m_position = weaponComponent.lock()->m_origin;
        // To
        vertices[2].m_position = weaponComponent.lock()->m_destination;
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(resolution.x), static_cast<F32>(resolution.y), 0.0f, -static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 colorLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(weaponComponent.lock()->m_hasHit ? Red : White));

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
