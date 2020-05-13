# coding: utf-8

import numpy as np
import logging
import robot

class MyRobot(robot.Robot):

    def tick(self, input):
        #input.linearVelocity = np.array([3, 5])
        input.angularVelocity = 45
        #input.shoot()
    
    def onHitWall(hitInfo):
        logging.info('Wall hit!')