#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <memory>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <functional>

#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdint>

#include <SDL.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <SDL_pixels.h>
#include <SDL_image.h>
#include <SDL_timer.h>

#include "Memory.h"
#include "Utils.h"
#include "Log.h"
#include "NetworkLog.h"

#endif //PCH_H
