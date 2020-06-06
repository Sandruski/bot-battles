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
        .def("reload", &InputComponent::Reload)
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

    py::class_<WeaponComponent, std::shared_ptr<WeaponComponent>>(m, "WeaponComponent")
        .def_property_readonly("primaryWeaponDamage", &WeaponComponent::GetPrimaryWeaponDamage)
        .def_property_readonly("secondaryWeaponDamage", &WeaponComponent::GetSecondaryWeaponDamage)
        .def_property_readonly("currentAmmo", &WeaponComponent::GetCurrentAmmo)
        .def_property_readonly("maxAmmo", &WeaponComponent::GetMaxAmmo)
        .def("hasAmmoBox", &WeaponComponent::HasAmmoBox)
        .def_property_readonly("ammoBoxAmmo", &WeaponComponent::GetAmmoBoxAmmo)
        .def_property_readonly("primaryWeaponRange", &WeaponComponent::GetPrimaryWeaponRange)
        .def_property_readonly("secondaryWeaponRange", &WeaponComponent::GetSecondaryWeaponRange)
        .def_property_readonly("primaryWeaponShootingTime", &WeaponComponent::GetPrimaryWeaponShootingTime)
        .def_property_readonly("primaryWeaponShootingCooldown", &WeaponComponent::GetPrimaryWeaponShootingCooldown)
        .def_property_readonly("secondaryWeaponShootingTime", &WeaponComponent::GetSecondaryWeaponShootingTime)
        .def_property_readonly("secondaryWeaponShootingCooldown", &WeaponComponent::GetSecondaryWeaponShootingCooldown)
        .def_property_readonly("reloadingTime", &WeaponComponent::GetReloadingTime)
        .def_property_readonly("reloadingCooldown", &WeaponComponent::GetReloadingCooldown);

    py::class_<HealthComponent, std::shared_ptr<HealthComponent>>(m, "HealthComponent")
        .def_property_readonly("currentHP", &HealthComponent::GetCurrentHP)
        .def_property_readonly("maxHP", &HealthComponent::GetMaxHP)
        .def("hasFirstAidBox", &HealthComponent::HasFirstAidBox)
        .def_property_readonly("firstAidBoxHP", &HealthComponent::GetFirstAidBoxHP)
        .def_property_readonly("healingTime", &HealthComponent::GetHealingTime)
        .def_property_readonly("healingCooldown", &HealthComponent::GetHealingCooldown);

    py::class_<MapComponent, std::shared_ptr<MapComponent>>(m, "MapComponent")
        .def("getTile", &MapComponent::GetTile)
        .def("getWorldPosition", &MapComponent::GetRealWorldPosition)
        //.def("getMapPosition", &MapComponent::RealWorldToMap)
        .def_property_readonly("tileCountX", &MapComponent::GetTileCountX)
        .def_property_readonly("tileCountY", &MapComponent::GetTileCountY);

    py::class_<MapComponent::Tile>(m, "Tile")
        .def_property_readonly("tileType", &MapComponent::Tile::GetTileType);

    py::enum_<MapComponent::Tile::TileType>(m, "TileType")
        .value("NONE", MapComponent::Tile::TileType::NONE)
        .value("FLOOR", MapComponent::Tile::TileType::FLOOR)
        .value("BOT_SPAWNER", MapComponent::Tile::TileType::BOT_SPAWNER)
        .value("WEAPON_SPAWNER", MapComponent::Tile::TileType::WEAPON_SPAWNER)
        .value("HEALTH_SPAWNER", MapComponent::Tile::TileType::HEALTH_SPAWNER)
        .export_values();

    py::class_<PhysicsComponent::Collision>(m, "CollisionEvent")
        .def_property_readonly("normalX", &PhysicsComponent::Collision::GetNormalX)
        .def_property_readonly("normalY", &PhysicsComponent::Collision::GetNormalY)
        .def_property_readonly("relativeVelocityX", &PhysicsComponent::Collision::GetRelativeVelocityX)
        .def_property_readonly("relativeVelocityY", &PhysicsComponent::Collision::GetRelativeVelocityY);
}
}

#endif
