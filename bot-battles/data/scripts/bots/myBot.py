# coding: utf-8

import numpy as np
import logging
import bot

class MyBot(bot.Bot):

    def tick(self, input):
        input.linearVelocityX = 100
        input.linearVelocityY = 30
        input.angularVelocity = 45
        # input.shoot()
    
    def onHitWall(self, input, collisionEvent):
        input.linearVelocityX = collisionEvent.relativeLinearVelocityX
        input.linearVelocityY = collisionEvent.relativeLinearVelocityY
        logging.info('Wall hit! %f %f', collisionEvent.relativeLinearVelocityX, collisionEvent.relativeLinearVelocityY)