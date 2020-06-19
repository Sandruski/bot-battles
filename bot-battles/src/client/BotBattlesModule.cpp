#ifndef __BOT_BATTLES_MODULE_H__
#define __BOT_BATTLES_MODULE_H__

#include "BotComponent.h"
#include "ColliderComponent.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "MapComponent.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "SightComponent.h"
#include "TransformComponent.h"
#include "UtilsClient.h"
#include "WeaponComponent.h"

// TODO: remove this. We already have it in the pchClient.h
#include <embed.h>
#include <pybind11.h>
#include <stl.h>

namespace py = pybind11;

namespace sand {

PYBIND11_EMBEDDED_MODULE(botbattles, m)
{
    m.doc() = "my module";

    m.def("log", &PyLog);

    py::class_<InputComponent, std::shared_ptr<InputComponent>>(m, "InputComponent")
        .def_property("linearVelocityX", &InputComponent::GetLinearVelocityX, &InputComponent::SetLinearVelocityX)
        .def_property("linearVelocityY", &InputComponent::GetLinearVelocityY, &InputComponent::SetLinearVelocityY)
        .def_property("angularVelocity", &InputComponent::GetAngularVelocity, &InputComponent::SetAngularVelocity)
        .def("shootPrimaryWeapon", &InputComponent::ShootPrimaryWeapon)
        .def("shootSecondaryWeapon", &InputComponent::ShootSecondaryWeapon)
        .def("reload", &InputComponent::Reload)
        .def("heal", &InputComponent::Heal);

    py::class_<TransformComponent, std::shared_ptr<TransformComponent>>(m, "TransformComponent")
        .def_property_readonly("position", &TransformComponent::GetPyPosition)
        .def_property_readonly("rotation", &TransformComponent::GetPyRotation)
        .def_property_readonly("direction", &TransformComponent::GetPyDirection);

    py::class_<RigidbodyComponent, std::shared_ptr<RigidbodyComponent>>(m, "RigidbodyComponent")
        .def_property_readonly("maxLinearVelocity", &RigidbodyComponent::GetPyMaxLinearVelocity)
        .def_property_readonly("maxAngularVelocity", &RigidbodyComponent::GetPyMaxAngularVelocity)
        .def_property_readonly("linearVelocity", &RigidbodyComponent::GetPyLinearVelocity)
        .def_property_readonly("angularVelocity", &RigidbodyComponent::GetPyAngularVelocity);

    py::class_<ColliderComponent, std::shared_ptr<ColliderComponent>>(m, "ColliderComponent")
        .def_property_readonly("size", &ColliderComponent::GetPySize);

    py::class_<WeaponComponent, std::shared_ptr<WeaponComponent>>(m, "WeaponComponent")
        .def_property_readonly("primaryWeaponDamage", &WeaponComponent::GetPyPrimaryWeaponDamage)
        .def_property_readonly("secondaryWeaponDamage", &WeaponComponent::GetPySecondaryWeaponDamage)
        .def_property_readonly("currentAmmo", &WeaponComponent::GetPyCurrentAmmo)
        .def_property_readonly("maxAmmo", &WeaponComponent::GetPyMaxAmmo)
        .def_property_readonly("ammoBoxAmmo", &WeaponComponent::GetPyAmmoBoxAmmo)
        .def_property_readonly("primaryWeaponRange", &WeaponComponent::GetPyPrimaryWeaponRange)
        .def_property_readonly("secondaryWeaponRange", &WeaponComponent::GetPySecondaryWeaponRange)
        .def_property_readonly("primaryWeaponShootingTime", &WeaponComponent::GetPyPrimaryWeaponShootingTime)
        .def_property_readonly("primaryWeaponShootingCooldown", &WeaponComponent::GetPyPrimaryWeaponShootingCooldown)
        .def_property_readonly("secondaryWeaponShootingTime", &WeaponComponent::GetPySecondaryWeaponShootingTime)
        .def_property_readonly("secondaryWeaponShootingCooldown", &WeaponComponent::GetPySecondaryWeaponShootingCooldown)
        .def_property_readonly("reloadingTime", &WeaponComponent::GetPyReloadingTime)
        .def_property_readonly("reloadingCooldown", &WeaponComponent::GetPyReloadingCooldown);

    py::class_<HealthComponent, std::shared_ptr<HealthComponent>>(m, "HealthComponent")
        .def_property_readonly("currentHP", &HealthComponent::GetPyCurrentHP)
        .def_property_readonly("maxHP", &HealthComponent::GetPyMaxHP)
        .def_property_readonly("firstAidBoxHP", &HealthComponent::GetPyFirstAidBoxHP)
        .def_property_readonly("healingTime", &HealthComponent::GetPyHealingTime)
        .def_property_readonly("healingCooldown", &HealthComponent::GetPyHealingCooldown);

    py::class_<SightComponent, std::shared_ptr<SightComponent>>(m, "SightComponent")
        .def_property_readonly("seenBotEntities", &SightComponent::GetPySeenBotEntities)
        .def("getSeenBotInfo", &SightComponent::GetPySeenBotInfo)
        .def_property_readonly("seenWeaponEntities", &SightComponent::GetPySeenWeaponEntities)
        .def("getSeenWeaponInfo", &SightComponent::GetPySeenWeaponInfo)
        .def_property_readonly("seenHealthEntities", &SightComponent::GetPySeenHealthEntities)
        .def("getSeenHealthInfo", &SightComponent::GetPySeenHealthInfo);

    py::class_<SightComponent::SeenBotInfo>(m, "SeenBotInfo")
        .def_property_readonly("transform", &SightComponent::SeenBotInfo::GetPyTransformComponent)
        .def_property_readonly("collider", &SightComponent::SeenBotInfo::GetPyColliderComponent)
        .def_property_readonly("rigidbody", &SightComponent::SeenBotInfo::GetPyRigidbodyComponent)
        .def_property_readonly("weapon", &SightComponent::SeenBotInfo::GetPyWeaponComponent)
        .def_property_readonly("health", &SightComponent::SeenBotInfo::GetPyHealthComponent)
        .def_property_readonly("action", &SightComponent::SeenBotInfo::GetPyBotComponent);

    py::class_<SightComponent::SeenWeaponInfo>(m, "SeenWeaponInfo")
        .def_property_readonly("transform", &SightComponent::SeenWeaponInfo::GetPyTransformComponent)
        .def_property_readonly("collider", &SightComponent::SeenWeaponInfo::GetPyColliderComponent)
        .def_property_readonly("weapon", &SightComponent::SeenWeaponInfo::GetPyWeaponComponent);

    py::class_<SightComponent::SeenHealthInfo>(m, "SeenHealthInfo")
        .def_property_readonly("transform", &SightComponent::SeenHealthInfo::GetPyTransformComponent)
        .def_property_readonly("collider", &SightComponent::SeenHealthInfo::GetPyColliderComponent)
        .def_property_readonly("health", &SightComponent::SeenHealthInfo::GetPyHealthComponent);

    py::class_<BotComponent, std::shared_ptr<BotComponent>>(m, "ActionComponent")
        .def_property_readonly("canPerformAction", &BotComponent::CanPyPerformAction);

    py::class_<MapComponent, std::shared_ptr<MapComponent>>(m, "MapComponent")
        .def("getTileType", &MapComponent::GetPyTileType)
        .def("getWorldPosition", &MapComponent::GetPyRealWorldPosition)
        .def("getMapPosition", &MapComponent::GetPyMapPosition)
        .def_property_readonly("tileCount", &MapComponent::GetPyTileCount);

    py::enum_<MapComponent::TileType>(m, "TileType")
        .value("NONE", MapComponent::TileType::NONE)
        .value("WALL", MapComponent::TileType::WALL)
        .value("FLOOR", MapComponent::TileType::FLOOR)
        .value("BOT_SPAWNER", MapComponent::TileType::BOT_SPAWNER)
        .value("WEAPON_SPAWNER", MapComponent::TileType::WEAPON_SPAWNER)
        .value("HEALTH_SPAWNER", MapComponent::TileType::HEALTH_SPAWNER)
        .export_values();

    py::class_<PhysicsComponent::Collision>(m, "CollisionEvent")
        .def_property_readonly("normal", &PhysicsComponent::Collision::GetPyNormal)
        .def_property_readonly("relativeVelocity", &PhysicsComponent::Collision::GetPyRelativeVelocity);
}
}

#endif
