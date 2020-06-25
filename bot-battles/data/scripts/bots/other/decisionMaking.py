# coding: utf-8

import glm
import heapq
import logging

import bot
import movement
import decisionMaking
import timer
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

# Movement
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

class TakeWeaponCover(GoToPosition):
    ...

class TakeHealthCover(GoToPosition):
    ...

class GoToLastKnownPosition(GoToPosition):
    ...

class GoToRandomPosition(GoToPosition):
    ...

class MoveTowardsBot(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo.transform == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = vector
        if glm.length(vector) > 0.0:
            direction = glm.normalize(vector)
        bot.agent.goToDirection((direction.x, direction.y))
        bot.agent.lookAt((direction.x, direction.y))

class MoveAwayFromBot(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo.transform == None:
            return

        vector = glm.vec2(bot.transform.position[0] - seenBotInfo.transform.position[0], bot.transform.position[1] - seenBotInfo.transform.position[1])
        direction = vector
        if glm.length(vector) > 0.0:
            direction = glm.normalize(vector)
        bot.agent.goToDirection((direction.x, direction.y))
        bot.agent.lookAt((direction.x, direction.y))

class LookAt(State):
    def __init__(self, worldDestinationDirection):
        self.worldDestinationDirection = worldDestinationDirection

    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.lookAt(self.worldDestinationDirection)

    def exit(self, bot):
        bot.agent.stopMove = False

class LookAtBullet(LookAt):
    ...

# Actions
class ShootPrimaryWeapon(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity
        self.timer = timer.Timer()

    def enter(self, bot):
        self.timer.start()

        bot.agent.stopMove = True

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo.transform == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = vector
        if glm.length(vector) > 0.0:
            direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if self.timer.read() < bot.delay:
            return

        if bot.agent.finishedRotate == True:
            input.shootPrimaryWeapon()

    def exit(self, bot):
        self.timer.stop()

        bot.agent.stopMove = False

class Reload(State):
    def __init__(self):
        self.timer = timer.Timer()

    def enter(self, bot):
        self.timer.start()

        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def update(self, bot, input):
        if self.timer.read() < bot.delay:
            return

        input.reload()

    def exit(self, bot):
        self.timer.stop()

        bot.agent.stopMove = False
        bot.agent.stopRotate = False

class ShootSecondaryWeapon(State):
    def __init__(self, seenBotEntity):
        self.seenBotEntity = seenBotEntity
        self.timer = timer.Timer()

    def enter(self, bot):
        self.timer.start()

        bot.agent.stopMove = True

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.seenBotEntity)
        if seenBotInfo.transform == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = vector
        if glm.length(vector) > 0.0:
            direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if self.timer.read() < bot.delay:
            return

        if bot.agent.finishedRotate == True:
            input.shootSecondaryWeapon()

    def exit(self, bot):
        self.timer.stop()

        bot.agent.stopMove = False

class Heal(State):
    def __init__(self):
        self.timer = timer.Timer()

    def enter(self, bot):
        self.timer.start()

        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def update(self, bot, input):
        if self.timer.read() < bot.delay:
            return

        input.heal()

    def exit(self, bot):
        self.timer.stop()

        bot.agent.stopMove = False
        bot.agent.stopRotate = False

class FSM:
    def __init__(self, bot):
        self.bot = bot
        self.currentState = None

    def changeCurrentState(self, newState):
        if newState == None:
            return

        logging.info('STATE: %s', newState.getName())

        if self.currentState != None:
           self.currentState.exit(self.bot)

        self.currentState = newState
        self.currentState.enter(self.bot)

    def updateCurrentState(self, input):
        if self.currentState != None:
            self.currentState.update(self.bot, input)

    def isCurrentState(self, state):
        if self.currentState == None:
            return False

        return self.currentState.getName() == state