# coding: utf-8

import numpy as np
import logging
import robot

class MyRobot(robot.Robot):

    def tick(self, input):
        input.linearVelocityX = 100
        input.linearVelocityY = 30
        input.angularVelocity = 45
        #input.shoot()
    
    def onHitWall(self, hitInfo):
        logging.info('Wall hit!')