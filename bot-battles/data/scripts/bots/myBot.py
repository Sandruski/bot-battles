# coding: utf-8

import numpy as np
import logging
import bot

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input):
        if self.wallHit == False:
            input.linearVelocityX = 100
            input.linearVelocityY = 30
        input.angularVelocity = 45
        input.shoot()
    
    def onHitWall(self, input, collisionEvent):
        self.wallHit = True
        input.linearVelocityX = collisionEvent.normalX * 100
        input.linearVelocityY = collisionEvent.normalY * 30
        logging.info('Wall hit! %f %f', collisionEvent.normalX, collisionEvent.normalY)