#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "Game.h"
#include "ComponentManager.h"
#include "ColliderComponent.h"

namespace sand {

    //----------------------------------------------------------------------------------------------------
    inline bool Raycast(Vec2 origin, Vec2 direction, F32 maxLength, Vec2& intersection)
    {
        bool ret = false;
        intersection = Vec2::positiveInfinity;

        Vec2 directionNormalized = direction.GetNormalized();
        std::vector<std::weak_ptr<ColliderComponent>> colliderComponents = g_game->GetComponentManager().GetComponents<ColliderComponent>();
        for (const auto& colliderComponent : colliderComponents)
        {
            SDL_Point originPoint = { static_cast<I32>(origin.x), static_cast<I32>(origin.y) };
            SDL_Rect colliderRect = colliderComponent.lock()->GetRect();
            if (SDL_PointInRect(&originPoint, &colliderRect))
            {
                continue;
            }

            I32 x1 = static_cast<I32>(origin.x);
            I32 y1 = static_cast<I32>(origin.y);
            I32 x2 = static_cast<I32>(origin.x + directionNormalized.x * maxLength);
            I32 y2 = static_cast<I32>(origin.y + directionNormalized.y * maxLength);
            if (SDL_IntersectRectAndLine(&colliderRect, &x1, &y1, &x2, &y2))
            {
                ret = true;
                ILOG("Has intersected");

                Vec2 newIntersection = Vec2(static_cast<F32>(x1), static_cast<F32>(y1));
                if (newIntersection < intersection)
                {
                    intersection = newIntersection;
                    ILOG("New closest point is %f, %f", intersection.x, intersection.y);
                }
            }
        }
        
        return ret;
    }
}

#endif