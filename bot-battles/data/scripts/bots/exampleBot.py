# coding: utf-8

import glm
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

class ExampleBot(bot.Bot):
    def __init__(self, transformComponent : TransformComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, rigidbodyComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
        self.graph = Graph(self.map)
        self.agent = movement.Agent(self)
        self.fsm = decisionMaking.FSM(self, decisionMaking.Idle())

    def tick(self, input : InputComponent):
        if self.action.canPerformAction:
            logging.info('change current state')
            self.fsm.changeCurrentState(decisionMaking.Shoot())
        else:
            logging.info('false')
        self.agent.update(input)
        self.fsm.updateCurrentState(input)

    def onHitWall(self, input, collision):
        #reflectionVector = glm.reflect(glm.vec2(-collision.relativeVelocity[0], -collision.relativeVelocity[1]), glm.vec2(collision.normal[0], collision.normal[1]))
        #input.linearVelocityX = reflectionVector.x
        #input.linearVelocityY = reflectionVector.y
        pass

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