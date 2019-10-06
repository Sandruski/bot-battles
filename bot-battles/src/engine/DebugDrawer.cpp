#include "DebugDrawer.h"

#include "Game.h"
#include "ModuleRenderer.h"

#include "Log.h"
#include "Memory.h"
#include "Defines.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	bool DebugDrawer::DrawQuad(const SDL_Rect& rect, const SDL_Color& color, bool isFilled)
	{
		SDL_Renderer* renderer = g_game->GetRenderer().GetRenderer();
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		int ret = isFilled ? SDL_RenderFillRect(renderer, &rect) : SDL_RenderDrawRect(renderer, &rect);
		if (ret == SDL_ERROR)
		{
			LOG("Quad could not be drawn! SDL Error: %s", SDL_GetError());
			return false;
		}
		
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool DebugDrawer::DrawLine(const SDL_Rect& rect, const SDL_Color& color)
	{
		SDL_Renderer* renderer = g_game->GetRenderer().GetRenderer();
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		
		int ret = SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.w, rect.h);
		if (ret == SDL_ERROR)
		{
			LOG("Line could not be drawn! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool DebugDrawer::DrawCircle(int x, int y, int radius, const SDL_Color& color)
	{
		SDL_Renderer* renderer = g_game->GetRenderer().GetRenderer();
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		SDL_Point points[360];
		F32 factor = (float)M_PI / 180.0f;
		for (U16 i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)(y + radius * sin(i * factor));
		}

		int ret = SDL_RenderDrawPoints(renderer, points, 360);
		if (ret == SDL_ERROR)
		{
			LOG("Circle could not be drawn! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	DebugDrawer::DebugDrawer()
	{
	}

	//----------------------------------------------------------------------------------------------------
	DebugDrawer::~DebugDrawer()
	{
	}
}