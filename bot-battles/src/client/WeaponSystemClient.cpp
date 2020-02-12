#include "WeaponSystemClient.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "LinkingContext.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "Intersection.h"
#include "Interpolation.h"
#include "WindowComponent.h"
#include "GameClient.h"
#include "ClientComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
    WeaponSystemClient::WeaponSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
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
                    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                    Vec2 position = transformComponent.lock()->GetPosition();
                    Vec2 rotation = transformComponent.lock()->GetRotation();
                    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
                    F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                    I32 x1 = static_cast<I32>(position.x);
                    I32 y1 = static_cast<I32>(position.y);
                    I32 x2 = static_cast<I32>(position.x + rotation.x * maxLength);
                    I32 y2 = static_cast<I32>(position.y + rotation.y * maxLength);
                    line = { x1, y1, x2, y2 };
                    shoot = true;
                    color = Blue;

                    Vec2 intersection;
                    std::weak_ptr<ColliderComponent> raycastColliderComponent = Raycast(position, rotation, maxLength, intersection);
                    if (!raycastColliderComponent.expired())
                    {
                        ILOG("HIT");
                        color = Red;
                    }
                }
            }
        }
        else 
        {
            std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
            coll = colliderComponent.lock()->GetRect();
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::DebugRender()
{
    //if (shoot) {
        DebugDrawer::DrawLine(line, color);
        //shoot = false;
    //}

    DebugDrawer::DrawQuad(coll, Orange);

    return true;
}
}
