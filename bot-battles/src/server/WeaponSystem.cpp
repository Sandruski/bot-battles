#include "WeaponSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "GameServer.h"
#include "ServerComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

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

        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        assert(clientProxy != nullptr);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

        for (U32 i = clientProxy->m_inputBuffer.m_front; i < clientProxy->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            if (inputComponent.m_isShooting) {
                //F32 dt = input.GetDt();
                ILOG("HIT");
            }

            // TODO: server side rewind
        }
    }

    return true;
}
}
