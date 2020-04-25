#ifndef __BOT_MODULE_H__
#define __BOT_MODULE_H__

#include <embed.h>
#include <pybind11.h>

namespace py = pybind11;

int calculate(int i, int j)
{
    return i + j;
}

PYBIND11_EMBEDDED_MODULE(bot, m)
{
    m.doc() = "my module";
    m.def("calculate", &calculate, "A function that calculates");
    //py::class_<InputComponent>(m, "InputComponent")
    //.def("setAngularAcceleration", &InputComponent::SetAngularAcceleration)
}

#endif
