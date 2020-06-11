# coding: utf-8

import glm
import heapq
import logging

import bot
import movement
import decisionMaking
from botbattles import TransformComponent
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

class GoToClosestWeaponSpawner(State):
    def enter(self, bot):
        weaponSpawnerTiles = bot.graph.getTilesOfType(TileType.WEAPON_SPAWNER)
        closestWeaponSpawnerTile = None
        for weaponSpawnerTile in weaponSpawnerTiles:
            if closestWeaponSpawnerTile == None:
                closestWeaponSpawnerTile = weaponSpawnerTile
                continue

            currentVector = glm.vec2(closestWeaponSpawnerTile[0] - bot.transform.position[0], closestWeaponSpawnerTile[1] - bot.transform.position[1])
            currentDistance = glm.length(currentVector)
            newVector = glm.vec2(weaponSpawnerTile[0] - bot.transform.position[0], weaponSpawnerTile[1] - bot.transform.position[1])
            newDistance = glm.length(newVector)
            if newDistance < currentDistance:
                closestWeaponSpawnerTile = weaponSpawnerTile
        
        if closestWeaponSpawnerTile == None:
            return

        worldDestinationPosition = bot.map.getWorldPosition(closestWeaponSpawnerTile)
        bot.agent.goTo(bot.transform.position, worldDestinationPosition)
        bot.agent.autoRotate = True

    def exit(self, bot):
        bot.agent.autoRotate = False

class ShootPrimaryWeapon(State):
    def __init__(self, targetEntity):
        self.targetEntity = targetEntity

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.targetEntity)
        if seenBotInfo == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if bot.agent.finishedRotate:
            input.shootPrimaryWeapon()

class Reload(State):
    def update(self, bot, input):
        input.Reload()

class ShootSecondaryWeapon(State):
    def __init__(self, targetEntity):
        self.targetEntity = targetEntity

    def update(self, bot, input):
        seenBotInfo = bot.sight.getSeenBotInfo(self.targetEntity)
        if seenBotInfo == None:
            return

        vector = glm.vec2(seenBotInfo.transform.position[0] - bot.transform.position[0], seenBotInfo.transform.position[1] - bot.transform.position[1])
        direction = glm.normalize(vector)
        bot.agent.lookAt((direction.x, direction.y))

        if bot.agent.finishedRotate:
            input.shootSecondaryWeapon()

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

        self.currentState = newState
        self.currentState.enter(self.bot)

    def updateCurrentState(self, input):
        if self.currentState != None:
            self.currentState.update(self.bot, input)