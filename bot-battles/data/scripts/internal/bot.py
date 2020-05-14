# coding: utf-8

class Bot:
    def __init__(self, transformComponent, rigidbodyComponent):
        self.transformComponent = transformComponent
        self.rigidbodyComponent = rigidbodyComponent

    def tick(self, input):
        pass

    def onHitWall(self, input, collisionEvent):
        pass