# coding: utf-8

import numpy as np
import glm
import logging
import bot

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input):
        if self.wallHit == False:
            input.linearVelocityX = 50
            input.linearVelocityY = 50
        input.angularVelocity = 45
        input.shoot()
    
    def onHitWall(self, input, collisionEvent):
        self.wallHit = True
        reflectionDirection = glm.reflect(glm.vec2(-collisionEvent.relativeVelocityX, -collisionEvent.relativeVelocityY), glm.vec2(collisionEvent.normalX, collisionEvent.normalY))
        input.linearVelocityX = reflectionDirection.x
        input.linearVelocityY = reflectionDirection.y
        logging.info('RV! %f %f', collisionEvent.relativeVelocityX, collisionEvent.relativeVelocityY)
        logging.info('Normal! %f %f', collisionEvent.normalX, collisionEvent.normalY)
        logging.info('Wall hit! %f %f', reflectionDirection.x, reflectionDirection.y)