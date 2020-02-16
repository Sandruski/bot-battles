#include "WeaponSystemClient.h"

#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "Interpolation.h"
#include "Intersection.h"
#include "LinkingContext.h"
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
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isLastMovePending) {
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
                    Vec2 position = transformComponent.lock()->GetPosition();
                    Vec2 rotation = transformComponent.lock()->GetRotation();
                    ILOG("CLIENT Pos is: %f %f", position.x, position.y);
                    ILOG("From frame %u to frame %u percentage %f", input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);

                    weaponComponent.lock()->m_origin = position;
                    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                    F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                    weaponComponent.lock()->m_destination = position + rotation * maxLength;
                    std::weak_ptr<ColliderComponent> intersection;
                    const bool hasIntersected = Raycast(position, rotation, maxLength, intersection);
                    if (hasIntersected) {
                        weaponComponent.lock()->m_hasHit = true;
                    } else {
                        weaponComponent.lock()->m_hasHit = false;
                    }
                }
            }

            break;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::DebugRender()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {

            std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
            DebugDrawer::DrawLine(weaponComponent.lock()->GetShotRect(), weaponComponent.lock()->m_hasHit ? Red : Black);

            break;
        }
    }

    return true;
}
}
