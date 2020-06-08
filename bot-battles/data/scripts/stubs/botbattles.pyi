# coding: utf-8

import enum

# Components
class InputComponent:
    maxLinearVelocity : float = ...
    maxAngularVelocity : float = ...
    linearVelocityX : float = ...
    linearVelocityY : float = ...
    angularVelocity : float = ...
    def shootPrimaryWeapon(self): ...
    def shootSecondaryWeapon(self): ...
    def reload(self): ...
    def heal(self): ...

class TransformComponent:
    position : tuple[float, float] = ...
    rotation : float = ...
    direction : tuple[float, float] = ...


class RigidbodyComponent:
    linearVelocity : tuple[float, float] = ...
    angularVelocity : float = ...

class WeaponComponent:
    primaryWeaponDamage : int = ...
    secondaryWeaponDamage : int = ...
    currentAmmo : int = ...
    maxAmmo : int = ...
    ammoBoxAmmo : int = ...
    primaryWeaponRange : float = ...
    secondaryWeaponRange : float = ...
    primaryWeaponShootingTime : float = ...
    primaryWeaponShootingCooldown : float = ...
    secondaryWeaponShootingTime : float = ...
    secondaryWeaponShootingCooldown : float = ...
    reloadingTime : float = ...
    reloadingCooldown : float = ...

class HealthComponent:
    currentHP : int = ...
    maxHP : int = ...
    firstAidBoxHP : int = ...
    healingTime : float = ...
    healingCooldown : float = ...

class SightComponent:
    seenBotEntities : list[int] = ...
    def getSeenBotInfo(self, botEntity : int) -> SeenBotInfo: ...
    seenWeaponEntities : list[int] = ...
    def getSeenWeaponInfo(self, weaponEntity : int) -> SeenWeaponInfo: ...
    seenHealthEntities : list[int] = ...
    def getSeenHealthInfo(self, healthEntity : int) -> SeenHealthInfo: ...

class SeenBotInfo:
    transform : TransformComponent = ...
    rigidbody : RigidbodyComponent = ...
    weapon : WeaponComponent = ...
    health : HealthComponent = ...

class SeenWeaponInfo:
    transform : TransformComponent = ...
    weapon : WeaponComponent = ...

class SeenHealthInfo:
    transform : TransformComponent = ...
    health : HealthComponent = ...

class MapComponent:
    def getTileType(self, mapPosition : tuple[int, int]) -> TileType: ...
    def getWorldPosition(self, mapPosition : tuple[int, int]) -> tuple[float, float]: ...
    def getMapPosition(self, worldPosition : tuple[float, float]) -> tuple[int, int]: ...
    tileCount : tuple[int, int] = ...

class TileType(enum.Enum):
    NONE : TileType = ...
    WALL : TileType = ...
    FLOOR : TileType = ...
    BOT_SPAWNER : TileType = ...
    WEAPON_SPAWNER : TileType = ...
    HEALTH_SPAWNER : TileType = ...

# Events
class CollisionEvent:
     normal : tuple[float, float] = ...
     relativeVelocity : tuple[float, float] = ...