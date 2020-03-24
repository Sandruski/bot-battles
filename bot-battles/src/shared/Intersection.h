#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "Game.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
inline bool Raycast(glm::vec2 position, glm::vec2 rotation, F32 maxLength, std::pair<Entity, std::weak_ptr<ColliderComponent>>& object, glm::vec2& intersection)
{
    bool ret = false;

    intersection = glm::vec2(FLT_MAX, FLT_MAX);

    std::vector<std::pair<Entity, std::weak_ptr<ColliderComponent>>> colliderComponents = g_game->GetComponentManager().GetComponents<ColliderComponent>();
    for (const auto& pair : colliderComponents) {
        SDL_Point originPoint = {
            static_cast<I32>(position.x),
            static_cast<I32>(position.y)
        };
        std::weak_ptr<ColliderComponent> colliderComponent = pair.second;
        SDL_Rect colliderRect = {
            static_cast<I32>(colliderComponent.lock()->m_position.x - colliderComponent.lock()->m_size.x / 2.0f),
            static_cast<I32>(colliderComponent.lock()->m_position.y - colliderComponent.lock()->m_size.y / 2.0f),
            static_cast<I32>(colliderComponent.lock()->m_size.x),
            static_cast<I32>(colliderComponent.lock()->m_size.y)
        };
        if (SDL_PointInRect(&originPoint, &colliderRect)) {
            continue;
        }

        I32 x1 = originPoint.x;
        I32 y1 = originPoint.y;
        I32 x2 = static_cast<I32>(position.x + rotation.x * maxLength);
        I32 y2 = static_cast<I32>(position.y + rotation.y * maxLength);
        if (SDL_IntersectRectAndLine(&colliderRect, &x1, &y1, &x2, &y2)) {
            ret = true;

            F32 distance = glm::distance2(intersection, position);
            glm::vec2 newIntersection = glm::vec2(static_cast<F32>(x1), static_cast<F32>(y1));
            F32 newDistance = glm::distance2(newIntersection, position);
            if (newDistance < distance) {
                intersection = newIntersection;
                object = pair;
            }
        }
    }

    return ret;
}
}

#endif
