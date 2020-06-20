# coding: utf-8

import logging

from botbattles import TransformComponent
from botbattles import ColliderComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import ActionComponent
from botbattles import MapComponent

class Bot:
    def __init__(self, transformComponent : TransformComponent, rigidbodyComponent : RigidbodyComponent, colliderComponent : ColliderComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        self.transform = transformComponent
        self.rigidbody = rigidbodyComponent
        self.collider = colliderComponent
        self.weapon = weaponComponent
        self.health = healthComponent
        self.sight = sightComponent
        self.action = actionComponent
        self.map = mapComponent

    def tick(self, input):
        pass

    def onSeenNewBot(self, input, seenBotEntity):
        pass

    def onSeenNewWeapon(self, input, seenWeaponEntity):
        pass

    def onSeenNewHealth(self, input, seenHealthEntity):
        pass

    def onSeenLostBot(self, input, seenBotEntity):
        pass

    def onSeenLostWeapon(self, input, seenWeaponEntity):
        pass

    def onSeenLostHealth(self, input, seenHealthEntity):
        pass

    def onWeaponPickedUp(self, input):
        pass

    def onHealthPickedUp(self, input):
        pass

    def onBulletHit(self, input):
        pass

    def onBulletMiss(self, input):
        pass

    def onReloaded(self, input, ammo):
        pass

    def onHitByBullet(self, input, health, direction):
        pass

    def onHealed(self, input, health):
        pass

    def onHitWall(self, input, collision):
        pass

    def onWin(self):
        pass

    def onLose(self):
        pass