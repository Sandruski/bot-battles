#include "DebugDrawer.h"

#include "Game.h"
#include "RendererComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool DebugDrawer::DrawQuad(const SDL_Rect& /*rect*/, const SDL_Color& /*color*/, bool /*isFilled*/)
{
    /*
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_SetRenderDrawColor(rendererComponent.m_renderer, color.r, color.g, color.b, color.a);

    int ret = isFilled ? SDL_RenderFillRect(rendererComponent.m_renderer, &rect) : SDL_RenderDrawRect(rendererComponent.m_renderer, &rect);
    if (ret == SDL_ERROR) {
        ELOG("Quad could not be drawn! SDL Error: %s", SDL_GetError());
        return false;
    }
    */
    return true;
}

//----------------------------------------------------------------------------------------------------
bool DebugDrawer::DrawLine(const SDL_Rect& /*rect*/, const SDL_Color& /*color*/)
{
    /*
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_SetRenderDrawColor(rendererComponent.m_renderer, color.r, color.g, color.b, color.a);

    int ret = SDL_RenderDrawLine(rendererComponent.m_renderer, rect.x, rect.y, rect.w, rect.h);
    if (ret == SDL_ERROR) {
        ELOG("Line could not be drawn! SDL Error: %s", SDL_GetError());
        return false;
    }
    */
    return true;
}

//----------------------------------------------------------------------------------------------------
bool DebugDrawer::DrawCircle(int /*x*/, int /*y*/, int /*radius*/, const SDL_Color& /*color*/)
{
    /*
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_SetRenderDrawColor(rendererComponent.m_renderer, color.r, color.g, color.b, color.a);

    SDL_Point points[360];
    F32 factor = (float)M_PI / 180.0f;
    for (U16 i = 0; i < 360; ++i) {
        points[i].x = (int)(x + radius * cos(i * factor));
        points[i].y = (int)(y + radius * sin(i * factor));
    }

    int ret = SDL_RenderDrawPoints(rendererComponent.m_renderer, points, 360);
    if (ret == SDL_ERROR) {
        ELOG("Circle could not be drawn! SDL Error: %s", SDL_GetError());
        return false;
    }
    */
    return true;
}
}
