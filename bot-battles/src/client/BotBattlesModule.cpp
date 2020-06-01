#ifndef __BOT_BATTLES_MODULE_H__
#define __BOT_BATTLES_MODULE_H__

#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "UtilsClient.h"

// TODO: remove this. We already have it in the pchClient.h
#include <embed.h>
#include <pybind11.h>

namespace py = pybind11;

namespace sand {

PYBIND11_EMBEDDED_MODULE(botbattles, m)
{
    m.doc() = "my module";

    m.def("log", &PyLog);

    py::class_<InputComponent, std::unique_ptr<InputComponent, py::nodelete>>(m, "InputComponent")
        .def_property_readonly("maxLinearVelocity", &InputComponent::GetMaxLinearVelocity)
        .def_property_readonly("maxAngularVelocity", &InputComponent::GetMaxAngularVelocity)
        .def_property("linearVelocityX", &InputComponent::GetLinearVelocityX, &InputComponent::SetLinearVelocityX)
        .def_property("linearVelocityY", &InputComponent::GetLinearVelocityY, &InputComponent::SetLinearVelocityY)
        .def_property("angularVelocity", &InputComponent::GetAngularVelocity, &InputComponent::SetAngularVelocity)
        .def("shootPrimaryWeapon", &InputComponent::ShootPrimaryWeapon)
        .def("shootSecondaryWeapon", &InputComponent::ShootSecondaryWeapon)
        .def("reloadSecondaryWeapon", &InputComponent::Reload);

    py::class_<TransformComponent, std::unique_ptr<TransformComponent, py::nodelete>>(m, "TransformComponent")
        .def_property_readonly("positionX", &TransformComponent::GetPositionX)
        .def_property_readonly("positionY", &TransformComponent::GetPositionY)
        .def_property_readonly("rotation", &TransformComponent::GetRotation)
        .def_property_readonly("directionX", &TransformComponent::GetDirectionX)
        .def_property_readonly("directionY", &TransformComponent::GetDirectionY);

    py::class_<RigidbodyComponent, std::unique_ptr<RigidbodyComponent, py::nodelete>>(m, "RigidbodyComponent")
        .def_property_readonly("linearVelocityX", &RigidbodyComponent::GetLinearVelocityX)
        .def_property_readonly("linearVelocityY", &RigidbodyComponent::GetLinearVelocityY)
        .def_property_readonly("angularVelocity", &RigidbodyComponent::GetAngularVelocity);

    py::class_<PhysicsComponent::Collision>(m, "CollisionEvent")
        .def_property_readonly("normalX", &PhysicsComponent::Collision::GetNormalX)
        .def_property_readonly("normalY", &PhysicsComponent::Collision::GetNormalY)
        .def_property_readonly("relativeVelocityX", &PhysicsComponent::Collision::GetRelativeVelocityX)
        .def_property_readonly("relativeVelocityY", &PhysicsComponent::Collision::GetRelativeVelocityY);
}
}

#endif
