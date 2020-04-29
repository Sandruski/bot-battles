#ifndef __UTILS_CLIENT_H__
#define __UTILS_CLIENT_H__

#include <pybind11.h>

namespace py = pybind11;

namespace sand {

void PyLog(const char* log);
}

#endif
