# coding: utf-8

#import numpy as np
import glm
import logging
import bot

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input):
        if self.wallHit == False:
            input.linearVelocityX = input.maxLinearVelocity
            input.linearVelocityY = input.maxLinearVelocity
        input.angularVelocity = input.maxAngularVelocity
        #input.shoot()
    
    def onHitWallEnter(self, input, collisionEvent):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collisionEvent.relativeVelocityX, -collisionEvent.relativeVelocityY), glm.vec2(collisionEvent.normalX, collisionEvent.normalY))
        input.linearVelocityX = reflectionVector.x
        input.linearVelocityY = reflectionVector.y
        logging.info('Wall hit enter')

    def onHitWallExit(self, input, collisionEvent):
        logging.info('Wall hit exit')

    def onSeenBotEnter(self, input):
        logging.info('Bot seen enter')

    def onSeenBotExit(self, input):
        logging.info('Bot seen exit')