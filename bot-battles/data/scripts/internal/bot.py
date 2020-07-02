# coding: utf-8

from botbattles import InputComponent
from botbattles import TransformComponent
from botbattles import ColliderComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import SeenBotInfo
from botbattles import SeenWeaponInfo
from botbattles import SeenHealthInfo
from botbattles import ActionComponent
from botbattles import MapComponent
from botbattles import TileType
from botbattles import CollisionEvent

class Bot:
    def __init__(self, 
                 transformComponent : TransformComponent, 
                 rigidbodyComponent : RigidbodyComponent, 
                 colliderComponent : ColliderComponent, 
                 weaponComponent : WeaponComponent, 
                 healthComponent : HealthComponent, 
                 sightComponent : SightComponent, 
                 actionComponent : ActionComponent, 
                 mapComponent : MapComponent):
        self.transform = transformComponent
        self.rigidbody = rigidbodyComponent
        self.collider = colliderComponent
        self.weapon = weaponComponent
        self.health = healthComponent
        self.sight = sightComponent
        self.action = actionComponent
        self.map = mapComponent

    def tick(self, input : InputComponent):
        pass

    def onSeenNewBot(self, input : InputComponent, seenBotEntity):
        pass

    def onSeenNewWeapon(self, input : InputComponent, seenWeaponEntity):
        pass

    def onSeenNewHealth(self, input : InputComponent, seenHealthEntity):
        pass

    def onSeenLostBot(self, input : InputComponent, seenBotEntity):
        pass

    def onSeenLostWeapon(self, input : InputComponent, seenWeaponEntity):
        pass

    def onSeenLostHealth(self, input : InputComponent, seenHealthEntity):
        pass

    def onWeaponPickedUp(self, input : InputComponent):
        pass

    def onHealthPickedUp(self, input : InputComponent):
        pass

    def onBulletHit(self, input : InputComponent):
        pass

    def onBulletMiss(self, input : InputComponent):
        pass

    def onReloaded(self, input : InputComponent, ammo):
        pass

    def onHitByBullet(self, input : InputComponent, health, direction):
        pass

    def onHealed(self, input : InputComponent, health):
        pass

    def onHitWall(self, input : InputComponent, collision : CollisionEvent):
        pass

    def onKill(self):
        pass

    def onDeath(self):
        pass