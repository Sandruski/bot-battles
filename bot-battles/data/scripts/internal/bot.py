# coding: utf-8

class Bot:
    def __init__(self, transformComponent, rigidbodyComponent):
        self.transformComponent = transformComponent
        self.rigidbodyComponent = rigidbodyComponent

    def tick(self, input):
        pass

    def onHitWall(self, input, collisionEvent):
        pass

    def onSeenNewBot(self, input):
        pass

    def onSeenLostBot(self, input):
        pass

    #def onHitByBullet(self, input):