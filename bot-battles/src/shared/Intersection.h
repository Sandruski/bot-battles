#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "Game.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
inline bool Raycast(Vec2 position, Vec2 rotation, F32 maxLength, std::weak_ptr<ColliderComponent>& intersection)
{
    std::vector<std::weak_ptr<ColliderComponent>> colliderComponents = g_game->GetComponentManager().GetComponents<ColliderComponent>();
    // TODO: instead of a vector of collider components also get the entities attached to this collider components to return the entity and component pair instead of only the component

    for (const auto& colliderComponent : colliderComponents) {
        SDL_Point originPoint = { static_cast<I32>(position.x), static_cast<I32>(position.y) };
        SDL_Rect colliderRect = colliderComponent.lock()->GetRect();
        if (SDL_PointInRect(&originPoint, &colliderRect)) {
            continue;
        }

        I32 x1 = originPoint.x;
        I32 y1 = originPoint.y;
        I32 x2 = static_cast<I32>(position.x + rotation.x * maxLength);
        I32 y2 = static_cast<I32>(position.y + rotation.y * maxLength);
        if (SDL_IntersectRectAndLine(&colliderRect, &x1, &y1, &x2, &y2)) {
            intersection = colliderComponent;
            return true;
        }
    }

    return false;
}
}

#endif
