# coding: utf-8

class Bot:
    def __init__(self, transformComponent, rigidbodyComponent):
        self.transformComponent = transformComponent
        self.rigidbodyComponent = rigidbodyComponent

    def tick(self, input):
        pass

    def onHitWallEnter(self, input, collisionEvent):
        pass

    def onHitWallExit(self, input, collisionEvent):
        pass

    def onSeenBotEnter(self, input):
        pass

    def onSeenBotExit(self, input):
        pass

    #def onHitByBullet(self, input):