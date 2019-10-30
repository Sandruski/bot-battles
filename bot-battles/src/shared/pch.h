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
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// SDL
#include <SDL_timer.h>

//----------------------------------------------------------------------------------------------------
// Math
#include "Vec2.h"
#include "Vec2I.h"
#include "Vec3.h"

// Utils
#include "Log.h"
#include "Memory.h"
#include "Timer.h"
#include "Utils.h"

#endif //PCH_H
