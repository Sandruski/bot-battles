# coding: utf-8

class Bot:
    def __init__(self, transformComponent, rigidbodyComponent):
        self.transformComponent = transformComponent
        self.rigidbodyComponent = rigidbodyComponent

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

    def onWeaponPicked(self, input):
        pass

    def onHealthPicked(self, input):
        pass

    def onBulletHit(self, input):
        pass

    def onBulletMiss(self, input):
        pass

    def onHitByBullet(self, input):
        pass

    def onHitWall(self, input, collisionEvent):
        pass