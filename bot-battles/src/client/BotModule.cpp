#ifndef __BOT_MODULE_H__
#define __BOT_MODULE_H__

#include "InputComponent.h"
#include "RigidbodyComponent.h"
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

    py::class_<InputComponent, std::unique_ptr<InputComponent, py::nodelete>>(m, "InputComponent")
        .def_property_readonly("maxLinearVelocity", &InputComponent::GetMaxLinearVelocity)
        .def_property_readonly("maxAngularVelocity", &InputComponent::GetMaxAngularVelocity)
        .def_property("linearVelocity", &InputComponent::GetLinearVelocity, &InputComponent::SetLinearVelocity)
        .def_property("angularVelocity", &InputComponent::GetAngularVelocity, &InputComponent::SetAngularVelocity)
        .def("shoot", &InputComponent::Shoot);

    py::class_<TransformComponent, std::unique_ptr<TransformComponent, py::nodelete>>(m, "TransformComponent")
        .def_property_readonly("position", &TransformComponent::GetPosition)
        .def_property_readonly("rotation", &TransformComponent::GetRotation);

    py::class_<RigidbodyComponent, std::unique_ptr<RigidbodyComponent, py::nodelete>>(m, "RigidbodyComponent")
        .def_property_readonly("linearVelocity", &RigidbodyComponent::GetLinearVelocity)
        .def_property_readonly("angularVelocity", &RigidbodyComponent::GetAngularVelocity);
    // TOWRITE: we need to read the velocity here besides from the input component because the one from the input component may be capped to max but we may be collisioning with a wall and the one from here will be 0
}
}

#endif
