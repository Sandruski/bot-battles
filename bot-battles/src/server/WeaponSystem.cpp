#include "WeaponSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "ServerComponent.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "Intersection.h"
#include "Interpolation.h"
#include "WindowComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystem::WeaponSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystem::Update()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID == INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            if (inputComponent.m_isShooting) {                /*
                LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
                const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

                if (serverComponent.m_isServerRewind) {
                    ILOG("SERVER SIDE REWIND");
                    U32 interpolationFromFrame = input.GetInterpolationFromFrame();
                    U32 interpolationToFrame = input.GetInterpolationToFrame();
                    F32 interpolationPercentage = input.GetInterpolationPercentage();

                    for (const auto& pair : newtorkIDToEntity) {
                        Entity remoteEntity = pair.second;
                        if (entity == remoteEntity) {
                            continue;
                        }

                        std::weak_ptr<TransformComponent> remoteTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
                        Transform fromTransform = remoteTransformComponent.lock()->m_transformBuffer.Get(interpolationFromFrame);
                        Transform toTransform = remoteTransformComponent.lock()->m_transformBuffer.Get(interpolationToFrame);
                        remoteTransformComponent.lock()->m_realPosition = remoteTransformComponent.lock()->m_position;
                        remoteTransformComponent.lock()->m_position = Lerp(fromTransform.m_position, toTransform.m_position, interpolationPercentage);
                    }
                }*/
                
                Vec2 position = transformComponent.lock()->GetPosition();
                Vec2 rotation = transformComponent.lock()->GetRotation();
                WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                I32 x1 = static_cast<I32>(position.x);
                I32 y1 = static_cast<I32>(position.y);
                I32 x2 = static_cast<I32>(position.x + rotation.x * maxLength);
                I32 y2 = static_cast<I32>(position.y + rotation.y * maxLength);
                line = { x1, y1, x2, y2 };
                shoot = true;
                color = Blue;
                Vec2 intersection;
                if (Raycast(position, rotation, maxLength, intersection))
                {
                    ILOG("HIT");
                    color = Red;
                }

                /*
                if (serverComponent.m_isServerRewind) {
                    ILOG("SERVER SIDE RE-REWIND");
                    for (const auto& pair : newtorkIDToEntity) {
                        Entity remoteEntity = pair.second;
                        if (entity == remoteEntity) {
                            continue;
                        }

                        std::weak_ptr<TransformComponent> remoteTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
                        remoteTransformComponent.lock()->m_position = remoteTransformComponent.lock()->m_realPosition;
                    }
                }*/
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystem::DebugRender()
{
    if (shoot) {
        DebugDrawer::DrawLine(line, color);
        shoot = false;
    }

    return true;
}
}
