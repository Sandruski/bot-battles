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
#include <SDL_ttf.h>

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

// Events
#include "Events.h"
#include "Observer.h"
#include "Subject.h"

// Defs
#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "NetDefs.h"
#include "SDLDefs.h"
#include "SystemDefs.h"
#include "WorkingDirDefs.h"

// Draw
#include "Colors.h"

//----------------------------------------------------------------------------------------------------

#endif //PCH_H
