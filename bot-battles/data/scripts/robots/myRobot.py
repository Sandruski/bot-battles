# coding: utf-8

import numpy as np
import logging
import robot

class MyRobot(robot.Robot):

    def tick(self, input):
        input.linearVelocity = np.array(100, 100)
        input.angularVelocity = 45
        input.shoot()
        logging.info('tick')