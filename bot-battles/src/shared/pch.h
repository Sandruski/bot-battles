#ifndef __PCH_H__
#define __PCH_H__

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//----------------------------------------------------------------------------------------------------
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <array>
#include <bitset>
#include <deque>
#include <filesystem>
#include <functional>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// SDL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>

// stb
#include <stb_image.h>

// gl3w
#include <GL/gl3w.h>

// glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/norm.hpp>
#include <gtx/vector_angle.hpp>

// imgui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_stdlib.h>

// rapidjson
#include <document.h>
#include <filereadstream.h>

// Box2D
#include <Box2D/Box2D.h>

// Optick
#include <optick.h>

//----------------------------------------------------------------------------------------------------
// Utils
#include "Log.h"
#include "Memory.h"

#include "MyTime.h"
#include "MyTimer.h"

#include "Utils.h"

// Math
#include "Interpolation.h"
#include "Random.h"

// Events
#include "Events.h"
#include "Observer.h"
#include "Subject.h"

// Networking
#include "CircularBuffer.h"
#include "InputMemoryStream.h"
#include "OutputMemoryStream.h"

// Defs
#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "NetDefs.h"
#include "PanelDefs.h"
#include "StateDefs.h"
#include "SystemDefs.h"
#include "WorkingDirDefs.h"

// Draw
#include "Colors.h"
#include "Layers.h"
#include "Shaders.h"

#endif
