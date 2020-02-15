#include "WeaponSystemServer.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "Interpolation.h"
#include "Intersection.h"
#include "LinkingContext.h"
#include "ServerComponent.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystemServer::WeaponSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Update()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID == INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            if (inputComponent.m_isShooting) {
                ILOG("Frame %u of playerID %u is shooting", i, playerID);
                if (serverComponent.m_isServerRewind) {
                    U32 interpolationFromFrame = input.GetInterpolationFromFrame();
                    U32 interpolationToFrame = input.GetInterpolationToFrame();
                    F32 interpolationPercentage = input.GetInterpolationPercentage();
                    Rewind(entity, interpolationFromFrame, interpolationToFrame, interpolationPercentage);
                }

                Vec2 position = transformComponent.lock()->GetPosition();
                Vec2 rotation = transformComponent.lock()->GetRotation();
                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                weaponComponent.lock()->m_destination = position + rotation * maxLength;
                std::weak_ptr<ColliderComponent> intersection;
                const bool hasIntersected = Raycast(position, rotation, maxLength, intersection);
                if (hasIntersected) {
                    weaponComponent.lock()->m_hasHit = true;
                } else {
                    weaponComponent.lock()->m_hasHit = false;
                }

                if (serverComponent.m_isServerRewind) {
                    Revert(entity);
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::DebugRender()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID == INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        DebugDrawer::DrawLine(weaponComponent.lock()->GetShotRect(), weaponComponent.lock()->m_hasHit ? Red : Black);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Rewind(Entity localEntity, U32 from, U32 to, F32 percentage)
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        if (!transformComponent.lock()->m_transformBuffer.IsEmpty()) {
            Transform fromTransform = transformComponent.lock()->m_transformBuffer.Get(from);
            Transform toTransform = transformComponent.lock()->m_transformBuffer.Get(to);
            Vec3 position = Lerp(fromTransform.m_position, toTransform.m_position, percentage);
            std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
            colliderComponent.lock()->m_position = { position.x, position.y };
            colliderComponent.lock()->m_shotPosition = colliderComponent.lock()->m_position;
            ILOG("SERVER collision is %f %f", colliderComponent.lock()->m_shotPosition.x, colliderComponent.lock()->m_shotPosition.y);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Revert(Entity localEntity)
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
        colliderComponent.lock()->m_position = transformComponent.lock()->GetPosition();
    }
}
}
