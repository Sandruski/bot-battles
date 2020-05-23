#include "WeaponSystemClient.h"

#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

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
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_isClientPrediction) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (!clientComponent.IsLocalEntity(entity)) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        if (clientComponent.m_isLastShootInputPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            U32 dirtyState = input.GetDirtyState();

            const bool hasShooting = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
            if (hasShooting) {
                glm::vec2 position = transformComponent.lock()->m_position;
                glm::vec2 rotation = transformComponent.lock()->GetDirection();

                weaponComponent.lock()->m_origin = position;
                ILOG("Origin: %f %f Rotation: %f", position.x, position.y, transformComponent.lock()->m_rotation);
                WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_currentResolution.x, windowComponent.m_currentResolution.y));
                weaponComponent.lock()->m_destination = position + rotation * maxLength;
                weaponComponent.lock()->m_hasHit = false;

                PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();
                PhysicsComponent::RaycastHit hitInfo;
                const bool hasIntersected = physicsComponent.Raycast(weaponComponent.lock()->m_origin, weaponComponent.lock()->m_destination, hitInfo);
                if (hasIntersected) {
                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(hitInfo.m_entity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destination = hitInfo.m_point;
                        ILOG("CLIENT POS %f %f ROT %f", hitEntityTransformComponent.lock()->m_position.x, hitEntityTransformComponent.lock()->m_position.y, hitEntityTransformComponent.lock()->m_rotation);
                    }

                    ILOG("Input %u", input.GetFrame());

                    std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(hitInfo.m_entity);
                    if (!hitEntityHealthComponent.expired()) {
                        weaponComponent.lock()->m_hasHit = true;
                        ILOG("HAS HIT CLIENT");
                    }
                }
            }

            clientComponent.m_isLastShootInputPending = false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!weaponComponent.lock()->m_isEnabled) {
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

        DebugDraw(rendererComponent, weaponComponent, color);
    }

    return true;
}
}
