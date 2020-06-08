# coding: utf-8

#import numpy as np
import glm
import logging
import bot
import heapq

from botbattles import TransformComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import MapComponent
from botbattles import InputComponent
from botbattles import TileType

class MyBot(bot.Bot):

    wallHit = False

    def __init__(self, transformComponent : TransformComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, rigidbodyComponent, weaponComponent, healthComponent, sightComponent, mapComponent)
        self.graph = Graph(self.map)
        self.pathFinder = PathFinder(self.graph)
        self.pathFollower = PathFollower()
        self.calculatePath = True
        self.originPosition = None
        self.destinationPosition = None
        self.originRotation = None
        self.destinationRotation = None

        self.minRotation = 5.0
        self.minPosition = 1.0

    def tick(self, input : InputComponent):
        if self.calculatePath:
            origin = self.map.getMapPosition(self.transform.position)
            destination = (5, 4)
            cameFrom = self.pathFinder.aStarSearch(origin, destination)
            self.pathFollower.path = self.pathFinder.reconstructPath(origin, destination, cameFrom)
            self.pathFollower.index = 0
            self.calculatePath = False
            self.destinationRotation = -45

        mapDestination = self.pathFollower.getCurrentMapDestination()
        worldDestination = self.map.getWorldPosition(mapDestination)
        #self.seek(input, worldDestination)adf
        self.align(input)
        
    def seek(self, input : InputComponent, worldDestination):
        vector = glm.vec2(worldDestination[0] - self.transform.position[0], worldDestination[1] - self.transform.position[1])
        if glm.length(vector) <= self.minPosition:
            input.linearVelocityX = 0
            input.linearVelocityY = 0
            return
        
        direction = glm.normalize(vector)
        acceleration = direction * input.maxLinearVelocity
        input.linearVelocityX = acceleration.x
        input.linearVelocityY = acceleration.y

    def align(self, input : InputComponent):
        rotation = self.destinationRotation - self.transform.rotation
        rotation = (rotation + 180.0) % 360.0 - 180.0
        absRotation = glm.abs(rotation)

        if absRotation <= self.minRotation:
            input.angularVelocity = 0.0
            return

        direction = rotation / absRotation
        angularAcceleration = direction * input.maxAngularVelocity
        input.angularVelocity = angularAcceleration

    def onHitWall(self, input, collision):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collision.relativeVelocity[0], -collision.relativeVelocity[1]), glm.vec2(collision.normal[0], collision.normal[1]))
        input.linearVelocityX = reflectionVector.x
        input.linearVelocityY = reflectionVector.y
        logging.info('onHitWall')

class PriorityQueue:
    def __init__(self):
        self.elements = []
    
    def empty(self):
        return len(self.elements) == 0
    
    def put(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self):
        return heapq.heappop(self.elements)[1]

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

class PathFinder:
    def __init__(self, graph):
        self.graph = graph

    def reconstructPath(self, origin, destination, cameFrom):
       current = destination
       path = []
       while current != origin:
           path.append(current)
           current = cameFrom[current]
       #path.append(origin)
       path.reverse()
       return path

    def calculateHeuristic(self, a, b):
        (x1, y1) = a
        (x2, y2) = b
        return abs(x1 - x2) + abs(y1 - y2)

    def aStarSearch(self, origin, destination):
        frontier = PriorityQueue()
        frontier.put(origin, 0)
        cameFrom = {}
        costSoFar = {}
        cameFrom[origin] = None
        costSoFar[origin] = 0
        
        while not frontier.empty():
            current = frontier.get()
            
            if current == destination:
                break
            
            for next in self.graph.getNeighbors(current, True):
                newCost = costSoFar[current] + self.graph.getCost(next)
                if next not in costSoFar or newCost < costSoFar[next]:
                    costSoFar[next] = newCost
                    priority = newCost + self.calculateHeuristic(destination, next)
                    frontier.put(next, priority)
                    cameFrom[next] = current
        
        return cameFrom

class PathFollower:
    def __init__(self):
        self.path = None
        self.index = 0

    def getCurrentMapDestination(self):
        return self.path[self.index]
