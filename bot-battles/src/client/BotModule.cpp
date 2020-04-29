#ifndef __BOT_MODULE_H__
#define __BOT_MODULE_H__

#include "InputComponent.h"
#include "TransformComponent.h"
#include "UtilsClient.h"

// TODO: remove this. We already have it in the pchClient.h
#include <embed.h>
#include <pybind11.h>

namespace py = pybind11;

namespace sand {

PYBIND11_EMBEDDED_MODULE(bot, m)
{
    m.doc() = "my module";

    m.def("log", &PyLog);

    py::class_<glm::vec2>(m, "vec2", py::buffer_protocol())
        .def_buffer([](glm::vec2& m) -> py::buffer_info {
            return py::buffer_info(
                glm::value_ptr(m), /* Pointer to buffer */
                sizeof(float), /* Size of one scalar */
                py::format_descriptor<float>::format(), /* Python struct-style format descriptor */
                1, /* Number of dimensions */
                { m.length() }, /* Buffer dimensions */
                { sizeof(float) } /* Strides (in bytes) for each index */
            );
        });

    py::class_<InputComponent>(m, "InputComponent")
        .def_readwrite("acceleration", &InputComponent::m_acceleration)
        .def_readwrite("angularAcceleration", &InputComponent::m_angularAcceleration);

    py::class_<TransformComponent>(m, "TransformComponent")
        .def_property_readonly("position", &TransformComponent::GetPosition)
        .def_property_readonly("rotation", &TransformComponent::GetRotation);
}
}

#endif
