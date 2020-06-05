# coding: utf-8

#import numpy as np
import glm
import logging
import bot

from botbattles import InputComponent

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input : InputComponent):
        if self.wallHit == False:
            input.linearVelocityX = int(self.map.getTileType(1, 1)) * 100
            input.linearVelocityY = 0
        input.angularVelocity = 0
        #input.shootSecondaryWeapon()
        input.reload()
        self.health.hasFirstAidBox()
        #input.shootPrimaryWeapon()
        #input.heal()

    def onWeaponPickedUp(self, input):
        logging.info('onWeaponPickedUp')

    def onReloaded(self, input):
        logging.info('onReloaded')

    def onBulletMiss(self, input):
        logging.info('onBulletMiss')

    def onHealthPickedUp(self, input):
        logging.info('onHealthPickedUp')

    def onHealed(self, input):
        logging.info('onHealed')

    def onHitWall(self, input, collisionEvent):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collisionEvent.relativeVelocityX, -collisionEvent.relativeVelocityY), glm.vec2(collisionEvent.normalX, collisionEvent.normalY))
        input.linearVelocityX = reflectionVector.x
        input.linearVelocityY = reflectionVector.y
        logging.info('onHitWall')
