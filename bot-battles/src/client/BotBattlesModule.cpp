#ifndef __BOT_BATTLES_MODULE_H__
#define __BOT_BATTLES_MODULE_H__

#include "HealthComponent.h"
#include "InputComponent.h"
#include "MapComponent.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "UtilsClient.h"
#include "WeaponComponent.h"

// TODO: remove this. We already have it in the pchClient.h
#include <embed.h>
#include <pybind11.h>
#include <stl.h>

namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::vector<int>); // TODO

namespace sand {

PYBIND11_EMBEDDED_MODULE(botbattles, m)
{
    m.doc() = "my module";

    m.def("log", &PyLog);

    py::class_<InputComponent, std::shared_ptr<InputComponent>>(m, "InputComponent")
        .def_property_readonly("maxLinearVelocity", &InputComponent::GetMaxLinearVelocity)
        .def_property_readonly("maxAngularVelocity", &InputComponent::GetMaxAngularVelocity)
        .def_property("linearVelocityX", &InputComponent::GetLinearVelocityX, &InputComponent::SetLinearVelocityX)
        .def_property("linearVelocityY", &InputComponent::GetLinearVelocityY, &InputComponent::SetLinearVelocityY)
        .def_property("angularVelocity", &InputComponent::GetAngularVelocity, &InputComponent::SetAngularVelocity)
        .def("shootPrimaryWeapon", &InputComponent::ShootPrimaryWeapon)
        .def("shootSecondaryWeapon", &InputComponent::ShootSecondaryWeapon)
        .def("reloadPrimaryWeapon", &InputComponent::Reload)
        .def("heal", &InputComponent::Heal);

    py::class_<TransformComponent, std::shared_ptr<TransformComponent>>(m, "TransformComponent")
        .def_property_readonly("positionX", &TransformComponent::GetPositionX)
        .def_property_readonly("positionY", &TransformComponent::GetPositionY)
        .def_property_readonly("rotation", &TransformComponent::GetRotation)
        .def_property_readonly("directionX", &TransformComponent::GetDirectionX)
        .def_property_readonly("directionY", &TransformComponent::GetDirectionY);

    py::class_<RigidbodyComponent, std::shared_ptr<RigidbodyComponent>>(m, "RigidbodyComponent")
        .def_property_readonly("linearVelocityX", &RigidbodyComponent::GetLinearVelocityX)
        .def_property_readonly("linearVelocityY", &RigidbodyComponent::GetLinearVelocityY)
        .def_property_readonly("angularVelocity", &RigidbodyComponent::GetAngularVelocity);

    py::class_<WeaponComponent, std::shared_ptr<WeaponComponent>>(m, "WeaponComponent");
    py::class_<HealthComponent, std::shared_ptr<HealthComponent>>(m, "HealthComponent");
    // TODO: weaponComponent, healthComponent

    py::class_<PhysicsComponent::Collision>(m, "CollisionEvent")
        .def_property_readonly("normalX", &PhysicsComponent::Collision::GetNormalX)
        .def_property_readonly("normalY", &PhysicsComponent::Collision::GetNormalY)
        .def_property_readonly("relativeVelocityX", &PhysicsComponent::Collision::GetRelativeVelocityX)
        .def_property_readonly("relativeVelocityY", &PhysicsComponent::Collision::GetRelativeVelocityY);

    py::class_<MapComponent, std::shared_ptr<MapComponent>>(m, "MapComponent")
        .def("getTileType", &MapComponent::GetTileType);

    py::enum_<MapComponent::TileType>(m, "TileType")
        .value("None", MapComponent::TileType::NONE)
        .value("Floor", MapComponent::TileType::FLOOR)
        .value("BotSpawner", MapComponent::TileType::BOT_SPAWNER)
        .value("WeaponSpawner", MapComponent::TileType::WEAPON_SPAWNER)
        .value("HealthSpawner", MapComponent::TileType::HEALTH_SPAWNER)
        .export_values();
}
}

#endif
