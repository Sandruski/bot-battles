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

class ExampleBot(bot.Bot):
    def __init__(self, transformComponent : TransformComponent, colliderComponent : ColliderComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, colliderComponent, rigidbodyComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
        self.graph = Graph(self.map)
        self.agent = movement.Agent(self)
        self.fsm = decisionMaking.FSM(self)
        self.lastSeenBotEntity = None

    def tick(self, input : InputComponent):
        if self.lastSeenBotEntity != None:
            seenBotInfo = self.sight.getSeenBotInfo(self.lastSeenBotEntity)
            distance = glm.distance(glm.vec2(seenBotInfo.transform.position[0], seenBotInfo.transform.position[1]),  glm.vec2(self.transform.position[0], self.transform.position[1]))
            colliderRange = self.collider.size[0] / 2.0 + seenBotInfo.collider.size[0] / 2.0
            
            if self.weapon.currentAmmo > 0:
                if distance <= self.weapon.primaryWeaponRange:
                    if distance >= colliderRange:
                        self.fsm.changeCurrentState(decisionMaking.ShootPrimaryWeapon(self.lastSeenBotEntity))
                    else:
                        self.fsm.changeCurrentState(decisionMaking.GoBackward())
                else:
                    self.fsm.changeCurrentState(decisionMaking.GoForward())    
            elif self.weapon.ammoBoxAmmo > 0:
                self.fsm.changeCurrentState(decisionMaking.Reload())
            else:
                if distance <= self.weapon.secondaryWeaponRange:
                    if distance >= colliderRange:
                        self.fsm.changeCurrentState(decisionMaking.ShootSecondaryWeapon(self.lastSeenBotEntity))
                    else:
                        self.fsm.changeCurrentState(decisionMaking.GoBackward())
                else:
                    self.fsm.changeCurrentState(decisionMaking.GoForward())
        else:
            if self.health.currentHP <= self.health.maxHP / 2.0:
                self.fsm.changeCurrentState(decisionMaking.GoToClosestHealthSpawner())
            elif self.weapon.ammoBoxAmmo <= 0:
                self.fsm.changeCurrentState(decisionMaking.GoToClosestWeaponSpawner())
            else:
                self.fsm.changeCurrentState(decisionMaking.GoToCenterMap())

        self.fsm.updateCurrentState(input)
        self.agent.update(input)

    def onSeenNewBot(self, input, seenBotEntity):
        logging.info('onSeenNewBot')
        self.lastSeenBotEntity = seenBotEntity

    def onSeenLostBot(self, input, seenBotEntity):
        logging.info('onSeenLostBot')
        self.lastSeenBotEntity = None

        seenBotInfo = self.sight.getSeenBotInfo(seenBotEntity)
        self.fsm.changeCurrentState(decisionMaking.GoToLastKnownPosition(seenBotInfo))

    def onHitByBullet(self, input, health, direction):
        logging.info('onHitByBullet')
        self.fsm.changeCurrentState(decisionMaking.LookAt((-direction[0], -direction[1])))

    def onHealthPickedUp(self, input):
        logging.info('onHealthPickedUp')
        self.fsm.changeCurrentState(decisionMaking.Heal())

class Graph:
    def __init__(self, map : MapComponent):
        self.map = map

    def getCost(self, mapPosition):
        return 1

    def isInBounds(self, mapPosition):
        return ((0 <= mapPosition[0] and mapPosition[0] < self.map.tileCount[0]) and (0 <= mapPosition[1] and mapPosition[1] < self.map.tileCount[1]))

    def isWalkable(self, mapPosition):
        tileType = self.map.getTileType(mapPosition)
        return self.isInBounds(mapPosition) and tileType != TileType.NONE and tileType != TileType.WALL

    def getNeighbors(self, mapPosition, diagonals):
        north = (mapPosition[0], mapPosition[1] + 1)
        south = (mapPosition[0], mapPosition[1] - 1)
        east = (mapPosition[0] + 1, mapPosition[1])
        west = (mapPosition[0] - 1, mapPosition[1])
        neighbors = [north, south, east, west]

        if diagonals:
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