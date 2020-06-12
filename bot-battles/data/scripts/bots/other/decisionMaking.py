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

    def exit(self, bot):
        bot.agent.autoRotate = False

class LookAt(State):
    def __init__(self, worldDestinationDirection):
        self.worldDestinationDirection = worldDestinationDirection

    def enter(self, bot):
        bot.agent.lookAt(self.worldDestinationDirection)

class GoToClosestWeaponSpawner(State):
    def enter(self, bot):
        weaponSpawnerTiles = bot.graph.getTilesOfType(TileType.WEAPON_SPAWNER)
        closestWeaponSpawnerTile = None
        for weaponSpawnerTile in weaponSpawnerTiles:
            if closestWeaponSpawnerTile == None:
                closestWeaponSpawnerTile = weaponSpawnerTile
                continue

            closestWeaponSpawnerWorldPosition = bot.map.getWorldPosition(closestWeaponSpawnerTile)
            closestWeaponSpawnerDistance = glm.distance(glm.vec2(closestWeaponSpawnerWorldPosition[0], closestWeaponSpawnerWorldPosition[1]),  glm.vec2(bot.transform.position[0], bot.transform.position[1]))
            weaponSpawnerWorldPosition = bot.map.getWorldPosition(weaponSpawnerTile)
            weaponSpawnerDistance = glm.distance(glm.vec2(weaponSpawnerWorldPosition[0], weaponSpawnerWorldPosition[1]),  glm.vec2(bot.transform.position[0], bot.transform.position[1]))
            if weaponSpawnerDistance < closestWeaponSpawnerDistance:
                closestWeaponSpawnerTile = weaponSpawnerTile
        
        if closestWeaponSpawnerTile == None:
            return

        worldDestinationPosition = bot.map.getWorldPosition(closestWeaponSpawnerTile)
        bot.agent.goToPosition(bot.transform.position, worldDestinationPosition)
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.autoRotate = False

class GoToClosestHealthSpawner(State):
    def enter(self, bot):
        healthSpawnerTiles = bot.graph.getTilesOfType(TileType.HEALTH_SPAWNER)
        closestHealthSpawnerTile = None
        for healthSpawnerTile in healthSpawnerTiles:
            if closestHealthSpawnerTile == None:
                closestHealthSpawnerTile = healthSpawnerTile
                continue

            closestHealthSpawnerWorldPosition = bot.map.getWorldPosition(closestHealthSpawnerTile)
            closestHealthSpawnerDistance = glm.distance(glm.vec2(closestHealthSpawnerWorldPosition[0], closestHealthSpawnerWorldPosition[1]),  glm.vec2(bot.transform.position[0], bot.transform.position[1]))
            healthSpawnerWorldPosition = bot.map.getWorldPosition(healthSpawnerTile)
            healthSpawnerDistance = glm.distance(glm.vec2(healthSpawnerWorldPosition[0], healthSpawnerWorldPosition[1]),  glm.vec2(bot.transform.position[0], bot.transform.position[1]))
            if healthSpawnerDistance < closestHealthSpawnerDistance:
                closestHealthSpawnerTile = healthSpawnerTile
        
        if closestHealthSpawnerTile == None:
            return

        worldDestinationPosition = bot.map.getWorldPosition(closestHealthSpawnerTile)
        bot.agent.goToPosition(bot.transform.position, worldDestinationPosition)
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.autoRotate = False

class GoToLastKnownPosition(State):
    def __init__(self, seenBotInfo):
        self.seenBotInfo = seenBotInfo

    def enter(self, bot):
        bot.agent.goToPosition(bot.transform.position, self.seenBotInfo.transform.position)
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.autoRotate = False

class GoForward(State):
    def enter(self, bot):
        direction = (bot.transform.direction[0], bot.transform.direction[1])
        bot.agent.goToDirection(direction)

class GoBackward(State):
    def enter(self, bot):
        direction = (-bot.transform.direction[0], -bot.transform.direction[1])
        bot.agent.goToDirection(direction)

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
        input.Reload()

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
            logging.info('finished rotating')
            input.shootSecondaryWeapon()
        else:
            logging.info('not finished rotating')

    def exit(self, bot):
        bot.agent.stopMove = False

class Heal(State):
    def enter(self, bot):
        bot.agent.stopMove = True
        bot.agent.stopRotate = True

    def update(self, bot, input):
        input.Heal()

    def enter(self, bot):
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
           if self.currentState.__class__.__name__ == newState.__class__.__name__:
               return

           self.currentState.exit(self.bot)

        logging.info('CHANGE STATE: %s', newState.__class__.__name__)
        self.currentState = newState
        self.currentState.enter(self.bot)

    def updateCurrentState(self, input):
        if self.currentState != None:
            self.currentState.update(self.bot, input)