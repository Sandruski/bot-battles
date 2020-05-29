# coding: utf-8

#import numpy as np
import glm
import logging
import bot

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input):
        if self.wallHit == False:
            input.linearVelocityX = 0
            input.linearVelocityY = input.maxLinearVelocity
        #input.angularVelocity = input.maxAngularVelocity
        #input.shootPrimaryWeapon()
        input.shootSecondaryWeapon()
    
    def onHitWallEnter(self, input, collisionEvent):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collisionEvent.relativeVelocityX, -collisionEvent.relativeVelocityY), glm.vec2(collisionEvent.normalX, collisionEvent.normalY))
        input.linearVelocityX = reflectionVector.x
        input.linearVelocityY = reflectionVector.y
        #logging.info('Wall hit enter')

    def onHitWallExit(self, input, collisionEvent):
        #logging.info('Wall hit exit')
        pass

    def onSeenNewBot(self, input):
        logging.info('Seen NEW bot')

    def onSeenLostBot(self, input):
        logging.info('Seen LOST bot')

    def onSeenNewAmmo(self, input):
        logging.info('Seen NEW ammo')

    def onSeenLostAmmo(self, input):
        logging.info('Seen LOST ammo')
