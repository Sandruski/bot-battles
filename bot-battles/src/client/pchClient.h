#ifndef __PCH_CLIENT_H__
#define __PCH_CLIENT_H__

#define _CLIENT

#ifdef _WIN64
#define USE_OPTICK 1
#else
#define USE_OPTICK 0
#endif

//----------------------------------------------------------------------------------------------------

#include "pch.h"

//----------------------------------------------------------------------------------------------------

#include <embed.h>
#include <pybind11.h>
#include <stl.h>

namespace py = pybind11;

#include "UtilsClient.h"

#endif
