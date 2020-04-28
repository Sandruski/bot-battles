# coding: utf-8

import bot
import numpy as np

import main

class MyBot(main.Bot):
    def __init__(self, transformComponent):
        super().__init__(transformComponent)

    def tick(self, input):
        position = np.array(self.transformComponent.position, copy=False)
        input.angularAcceleration = position[0]