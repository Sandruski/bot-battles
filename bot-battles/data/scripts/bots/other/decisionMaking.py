# coding: utf-8

import logging

import bot
from botbattles import TransformComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import ActionComponent
from botbattles import MapComponent

class State:
    def __init__(self):
        pass

    def enter(self, bot):
        pass

    def update(self, bot, input):
        pass

    def exit(self, bot):
        pass

class Idle(State):
    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def exit(self, bot):
        bot.agent.stopMove = False
        bot.agent.stopRotate = False

class GoTo(State):
    def __init__(self, worldOriginPosition, worldDestinationPosition):
        self.worldOriginPosition = worldOriginPosition
        self.worldDestinationPosition = worldDestinationPosition

    def enter(self, bot):
        bot.agent.goTo(self.worldOriginPosition, self.worldDestinationPosition)
        bot.agent.autoRotate = True

class Shoot(State):
    def update(self, bot, input):
        if bot.weapon.currentAmmo > 0:
            input.shootPrimaryWeapon()
        else:
            if bot.weapon.ammoBoxAmmo > 0:
                input.reload()
            else:
                input.shootSecondaryWeapon()

class FSM:
    def __init__(self, bot, initialState):
        self.bot = bot
        self.currentState = initialState

    def changeCurrentState(self, newState):
        if self.currentState != None and self.bot != None:
            self.currentState.exit(self.bot)

        self.currentState = newState

        if self.currentState != None and self.bot != None:
            self.currentState.enter(self.bot)

    def updateCurrentState(self, input):
        if self.currentState != None and self.bot != None:
            self.currentState.update(self.bot, input)