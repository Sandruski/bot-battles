# coding: utf-8
import bot
import numpy as np

myBot = None

def init(transformComponent):
    global myBot
    myBot = Bot(transformComponent)

def update(input):
    myBot.tick(input)

class Bot:
    def __init__(self, transformComponent):
        self.transformComponent = transformComponent

    def tick(self, input):
        position = np.array(self.transformComponent.position, copy=False)
        position[0] = 10
        input.angularAcceleration = position[0]