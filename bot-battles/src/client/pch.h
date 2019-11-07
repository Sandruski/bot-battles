#ifndef __PCH_H__
#define __PCH_H__

//----------------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <array>
#include <bitset>
#include <deque>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// SDL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

//----------------------------------------------------------------------------------------------------
// Math
#include "Vec2.h"
#include "Vec2I.h"
#include "Vec3.h"

// Utils
#include "Log.h"
#include "Memory.h"
#include "Time.h"
#include "Timer.h"
#include "Utils.h"

//----------------------------------------------------------------------------------------------------
// Project
#include "Colors.h"

#endif //PCH_H

//#include <SDL.h>
//#include <SDL_mouse.h>
//#include <SDL_scancode.h>
//#include <SDL_pixels.h>
//#include <SDL_image.h>
