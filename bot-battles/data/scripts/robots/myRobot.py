# coding: utf-8

import numpy as np
import logging

import bot

import robot

class MyRobot(robot.Robot):
    def tick(self, input):
        position = np.array(self.transformComponent.position, copy=False)
        input.angularAcceleration = position[0]
        logging.error('my error')
        logging.info('hello world')