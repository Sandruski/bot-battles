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
            input.linearVelocityY = 0
        input.angularVelocity = 0
        input.shootPrimaryWeapon()
        #input.shootSecondaryWeapon()
        #input.reloadPrimaryWeapon()
        #input.heal()

    def onSeenNewBot(self, input):
        logging.info('onSeenNewBot')

    def onSeenNewWeapon(self, input):
        logging.info('onSeenNewWeapon')

    def onSeenNewHealth(self, input):
        logging.info('onSeenNewHealth')

    def onSeenLostBot(self, input):
        logging.info('onSeenLostBot')

    def onSeenLostWeapon(self, input):
        logging.info('onSeenLostWeapon')

    def onSeenLostHealth(self, input):
        logging.info('onSeenLostHealth')

    def onWeaponPicked(self, input):
        logging.info('onWeaponPicked')

    def onHealthPicked(self, input):
        logging.info('onHealthPicked')

    def onBulletHit(self, input):
        logging.info('onBulletHit')

    def onBulletMiss(self, input):
        logging.info('onBulletMiss')

    def onHitByBullet(self, input):
        logging.info('onHitByBullet')

    def onHitWall(self, input, collisionEvent):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collisionEvent.relativeVelocityX, -collisionEvent.relativeVelocityY), glm.vec2(collisionEvent.normalX, collisionEvent.normalY))
        input.linearVelocityX = reflectionVector.x
        input.linearVelocityY = reflectionVector.y
        logging.info('onHitWall')
