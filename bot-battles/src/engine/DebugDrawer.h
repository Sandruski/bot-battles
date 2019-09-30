#ifndef __DEBUG_DRAWER_H__
#define __DEBUG_DRAWER_H__

struct SDL_Rect;
struct SDL_Color;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class DebugDrawer
	{
	public:
		static bool DrawQuad(const SDL_Rect& rect, const SDL_Color& color, bool isFilled = true);
		static bool DrawLine(const SDL_Rect& rect, const SDL_Color& color);
		static bool DrawCircle(int x, int y, int radius, const SDL_Color& color);

	private:
		DebugDrawer();
		~DebugDrawer();
	};
}

#endif