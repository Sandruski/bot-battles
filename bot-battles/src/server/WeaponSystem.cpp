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

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystem::WeaponSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    //m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER); // TODO: uncomment
    //m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
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
            if (inputComponent.m_isShooting) {
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
                }
                
                /*
                if (Raycast(transformComponent.lock()->m_position, ))
                {
                    ILOG("HIT");
                }*/
                shoot = true;

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
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystem::DebugRender()
{
    if (shoot) {
        DebugDrawer::DrawLine(line, Blue);
        shoot = false;
    }

    return true;
}
}
