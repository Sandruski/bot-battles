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
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::PLAY) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (clientComponent.m_isLastInputWeaponPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            const InputComponent& inputComponent = input.GetInputComponent();

            if (inputComponent.m_isShooting) {
                LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
                const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

                for (const auto& pair : newtorkIDToEntity) {
                    Entity remoteEntity = pair.second;
                    if (entity == remoteEntity) {
                        continue;
                    }

                    std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
                    colliderComponent.lock()->m_shotPosition = colliderComponent.lock()->m_position;
                    ILOG("CLIENT Rewind is %f %f", colliderComponent.lock()->m_shotPosition.x, colliderComponent.lock()->m_shotPosition.y);
                }

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);

                // TODO: CHECK RAYS BETWEEN SERVER AND CLIENT
                glm::vec2 position = transformComponent.lock()->GetPosition();
                glm::vec2 rotation = transformComponent.lock()->GetRotation();
                ILOG("CLIENT Pos is: %f %f", position.x, position.y);
                ILOG("From frame %u to frame %u percentage %f", input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);

                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                weaponComponent.lock()->m_destination = position + rotation * maxLength;
                std::pair<Entity, std::weak_ptr<ColliderComponent>> intersection;
                const bool hasIntersected = Raycast(position, rotation, maxLength, intersection);
                if (hasIntersected) {
                    weaponComponent.lock()->m_hasHit = true;
                } else {
                    weaponComponent.lock()->m_hasHit = false;
                }
            }

            clientComponent.m_isLastInputWeaponPending = false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::DebugRender()
{
    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        // From
        vertices[0].m_position = weaponComponent.lock()->m_origin;
        // To
        vertices[2].m_position = weaponComponent.lock()->m_destination;
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_resolution.x), static_cast<F32>(windowComponent.m_resolution.y), 0.0f, -1.0f, 1.0f);
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 colorLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(weaponComponent.lock()->m_hasHit ? Red : Blue));

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
