# coding: utf-8

import glm
import heapq
import logging

import bot
import movement
import decisionMaking
from botbattles import TransformComponent
from botbattles import ColliderComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import ActionComponent
from botbattles import MapComponent
from botbattles import InputComponent
from botbattles import TileType

class State:
    def __init__(self):
        pass

    def getName(self):
        return self.__class__.__name__

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

class GoToPosition(State):
    def __init__(self, worldOriginPosition, worldDestinationPosition):
        self.worldOriginPosition = worldOriginPosition
        self.worldDestinationPosition = worldDestinationPosition

    def enter(self, bot):
        bot.agent.goToPosition(self.worldOriginPosition, self.worldDestinationPosition)
        bot.agent.followPath = True
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.followPath = False
        bot.agent.autoRotate = False

class GoToWeaponSpawner(GoToPosition):
    ...

class GoToHealthSpawner(GoToPosition):
    ...

class GoToLastKnownPosition(GoToPosition):
    ...

class GoToCenterMap(GoToPosition):
    ...

class GoToDirection(State):
    def __init__(self, worldDestinationDirection):
        self.worldDestinationDirection = worldDestinationDirection

    def enter(self, bot):
        bot.agent.goToDirection(worldDestinationDirection)
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.autoRotate = False

class GoForward(GoToDirection):
    ...

class GoBackward(GoToDirection):
    ...

class LookAt(State):
    def __init__(self, worldDestinationDirection):
        self.worldDestinationDirection = worldDestinationDirection

    def enter(self, bot):
        bot.agent.lookAt(self.worldDestinationDirection)

class Rotate(State):
    def __init__(self, angularVelocity):
        self.angularVelocity = angularVelocity

    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.angularVelocity = self.angularVelocity
        bot.agent.useAngularVelocity = True

    def exit(self, bot):
        bot.agent.stopMove = False
        bot.agent.useAngularVelocity = False

class ShootPrimaryWeapon(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity

    def enter(self, bot):
        bot.agent.stopMove = True

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if bot.agent.finishedRotate:
            input.shootPrimaryWeapon()

    def exit(self, bot):
        bot.agent.stopMove = False

class Reload(State):
    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def update(self, bot, input):
        input.reload()

    def exit(self, bot):
        bot.agent.stopMove = False
        bot.agent.stopRotate = False

class ShootSecondaryWeapon(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity

    def enter(self, bot):
        bot.agent.stopMove = True

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if bot.agent.finishedRotate:
            input.shootSecondaryWeapon()

    def exit(self, bot):
        bot.agent.stopMove = False

class Heal(State):
    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def update(self, bot, input):
        input.heal()

    def exit(self, bot):
        bot.agent.stopMove = False
        bot.agent.stopRotate = False

class FSM:
    def __init__(self, bot):
        self.bot = bot
        self.currentState = None

    def changeCurrentState(self, newState):
        if newState == None:
            return

        if self.currentState != None:
           self.currentState.exit(self.bot)

        self.currentState = newState
        self.currentState.enter(self.bot)

        logging.info('NEW STATE: %s', self.currentState.getName())

    def updateCurrentState(self, input):
        if self.currentState != None:
            self.currentState.update(self.bot, input)

    def isCurrentState(self, state):
        if self.currentState == None:
            return False

        if self.currentState.getName() == state:
            return True

        return False