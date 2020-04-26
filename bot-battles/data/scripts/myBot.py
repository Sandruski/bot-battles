# coding: utf-8
import bot
import numpy as np

def tick(input):
    acceleration = np.array(input.acceleration, copy=False)
    acceleration[:] = [5,10]
    input.angularAcceleration = 8