# coding: utf-8

class Bot:
    def __init__(self, transformComponent, rigidbodyComponent, weaponComponent, healthComponent, mapComponent):
        self.transform = transformComponent
        self.rigidbody = rigidbodyComponent
        self.weapon = weaponComponent
        self.health = healthComponent
        self.map = mapComponent

    def tick(self, input):
        pass

    def onSeenNewBot(self, input):
        pass

    def onSeenNewWeapon(self, input):
        pass

    def onSeenNewHealth(self, input):
        pass

    def onSeenLostBot(self, input):
        pass

    def onSeenLostWeapon(self, input):
        pass

    def onSeenLostHealth(self, input):
        pass

    def onWeaponPickedUp(self, input):
        pass

    def onHealthPickedUp(self, input):
        pass

    def onBulletHit(self, input):
        pass

    def onBulletMiss(self, input):
        pass

    def onReloaded(self, input):
        pass

    def onHitByBullet(self, input):
        pass

    def onHealed(self, input):
        pass

    def onHitWall(self, input, collisionEvent):
        pass