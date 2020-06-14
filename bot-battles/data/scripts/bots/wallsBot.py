# coding: utf-8

import glm
import heapq
import logging
import random

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

class WallsBot(bot.Bot):
    def __init__(self, transformComponent : TransformComponent, colliderComponent : ColliderComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, colliderComponent, rigidbodyComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
        self.graph = Graph(self.map)
        self.agent = movement.Agent(self)
        self.fsm = decisionMaking.FSM(self)
        self.lastSeenBotEntity = None
        self.lastWallDirection = (0.5, 0.5)

    def tick(self, input : InputComponent):
        if self.action.canPerformAction:
            self.think()
        
        self.fsm.updateCurrentState(input)
        self.agent.update(input)

    def onSeenNewBot(self, input, seenBotEntity):
        logging.info('EVENT: onSeenNewBot')
        self.lastSeenBotEntity = seenBotEntity

    def onSeenLostBot(self, input, seenBotEntity):
        logging.info('EVENT: onSeenLostBot')
        self.lastSeenBotEntity = None

    def onHitWall(self, input, collision):
        logging.info('EVENT: onHitWall')

        reflectionDirection = glm.reflect(glm.vec2(-collision.relativeVelocity[0], -collision.relativeVelocity[1]), glm.vec2(collision.normal[0], collision.normal[1]))
        self.lastWallDirection = reflectionDirection

    def think(self):
        if self.lastSeenBotEntity != None:
            seenBotInfo = self.sight.getSeenBotInfo(self.lastSeenBotEntity)
            distance = glm.distance(glm.vec2(seenBotInfo.transform.position[0], seenBotInfo.transform.position[1]),  glm.vec2(self.transform.position[0], self.transform.position[1]))
            colliderRange = self.collider.size[0] / 2.0 + seenBotInfo.collider.size[0] / 2.0

            # Health
            if self.health.currentHP < self.health.maxHP / 2.0 or self.health.currentHP < seenBotInfo.health.currentHP / 2.0 and self.health.firstAidBoxHP > 0:
                if self.fsm.isCurrentState("Heal") == False:
                    self.fsm.changeCurrentState(decisionMaking.Heal())
            # Weapon
            elif self.weapon.currentAmmo > 0 and distance <= self.weapon.primaryWeaponRange and distance >= colliderRange:
                if self.fsm.isCurrentState("ShootPrimaryWeapon") == False:
                    self.fsm.changeCurrentState(decisionMaking.ShootPrimaryWeapon(self.lastSeenBotEntity))
            elif self.weapon.ammoBoxAmmo > 0:
                if self.fsm.isCurrentState("Reload") == False:
                    self.fsm.changeCurrentState(decisionMaking.Reload())
            elif distance <= self.weapon.secondaryWeaponRange and distance >= colliderRange:
                if self.fsm.isCurrentState("ShootSecondaryWeapon") == False:
                    self.fsm.changeCurrentState(decisionMaking.ShootSecondaryWeapon(self.lastSeenBotEntity))
            # Other
            else:
                worldDestinationDirection = self.lastWallDirection
                self.fsm.changeCurrentState(decisionMaking.GoToDirection(worldDestinationDirection))
        else:
            # Health
            if self.health.currentHP < self.health.maxHP / 2.0 and self.health.firstAidBoxHP > 0:
                if self.fsm.isCurrentState("Heal") == False:
                    self.fsm.changeCurrentState(decisionMaking.Heal())
            # Weapon
            elif (self.weapon.maxAmmo == 0 or self.weapon.currentAmmo < self.weapon.maxAmmo) and self.weapon.ammoBoxAmmo > 0:
                if self.fsm.isCurrentState("Reload") == False:
                    self.fsm.changeCurrentState(decisionMaking.Reload())
            # Other
            else:
                worldDestinationDirection = self.lastWallDirection
                self.fsm.changeCurrentState(decisionMaking.GoToDirection(worldDestinationDirection))

class Graph:
    def __init__(self, map : MapComponent):
        self.map = map

    def getCost(self, mapPosition):
        return 1

    def isInBounds(self, mapPosition):
        return ((0 <= mapPosition[0] and mapPosition[0] < self.map.tileCount[0]) and (0 <= mapPosition[1] and mapPosition[1] < self.map.tileCount[1]))

    def isWalkable(self, mapPosition):
        return self.isInBounds(mapPosition) and self.map.getTileType(mapPosition) != TileType.NONE and self.map.getTileType(mapPosition) != TileType.WALL

    def getNeighbors(self, mapPosition, diagonals):
        north = (mapPosition[0], mapPosition[1] + 1)
        south = (mapPosition[0], mapPosition[1] - 1)
        east = (mapPosition[0] + 1, mapPosition[1])
        west = (mapPosition[0] - 1, mapPosition[1])
        neighbors = [north, south, east, west]

        if diagonals == True:
            northEast = (mapPosition[0] + 1, mapPosition[1] + 1)
            northWest = (mapPosition[0] - 1, mapPosition[1] + 1)
            southEast = (mapPosition[0] + 1, mapPosition[1] - 1)
            southWest = (mapPosition[0] - 1, mapPosition[1] - 1)
            neighbors.append(northEast)
            neighbors.append(northWest)
            neighbors.append(southEast)
            neighbors.append(southWest)
        
        neighbors = filter(self.isWalkable, neighbors)
        return neighbors

    def getTilesOfType(self, type):
        tiles = []

        for i in range(0, self.map.tileCount[0]):
            for j in range(0, self.map.tileCount[1]):
                mapPosition = (i, j)
                tileType = self.map.getTileType(mapPosition)
                if tileType == type:
                    tiles.append(mapPosition)
        return tiles