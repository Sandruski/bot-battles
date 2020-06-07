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
from botbattles import MapComponent
from botbattles import InputComponent
from botbattles import TileType

class MyBot(bot.Bot):

    wallHit = False

    def __init__(self, transformComponent : TransformComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, rigidbodyComponent, weaponComponent, healthComponent, mapComponent)
        self.pathfinder = Pathfinder(Graph(self.map))
        self.path = None
        self.pathIndex = None
        self.calculatePath = True

    def tick(self, input : InputComponent):
        if self.calculatePath:
            origin = self.map.getMapPosition(self.transform.position)
            destination = (5, 5)
            cameFrom = self.pathfinder.aStarSearch(origin, destination)
            self.path = self.pathfinder.reconstructPath(origin, destination, cameFrom)
            self.pathIndex = 0
            self.calculatePath = False

        currentTile = self.map.getMapPosition(self.transform.position)
        nextTile = self.path[self.pathIndex]
        if currentTile == nextTile and len(self.path) < self.pathIndex - 1:
            self.pathIndex += 1
        nextPosition = self.map.getWorldPosition(nextTile)
        direction = []
        direction.append(nextPosition[0] - self.transform.position[0])   
        direction.append(nextPosition[1] - self.transform.position[1])
        glmDirection = glm.vec2(direction[0], direction[1])
        glmDirection = glm.normalize(glmDirection)
        direction[0] = glmDirection.x
        direction[1] = glmDirection.y
        input.linearVelocityX = direction[0] * input.maxLinearVelocity
        input.linearVelocityY = direction[1] * input.maxLinearVelocity

    def onWeaponPickedUp(self, input):
        logging.info('onWeaponPickedUp')

    def onReloaded(self, input):
        logging.info('onReloaded')

    def onBulletMiss(self, input):
        logging.info('onBulletMiss')

    def onHealthPickedUp(self, input):
        logging.info('onHealthPickedUp')

    def onHealed(self, input):
        logging.info('onHealed')

    def onHitWall(self, input, collisionEvent):
        self.wallHit = True
        reflectionVector = glm.reflect(glm.vec2(-collisionEvent.relativeVelocity[0], -collisionEvent.relativeVelocity[1]), glm.vec2(collisionEvent.normal[0], collisionEvent.normal[1]))
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
    def __init__(self, map):
        self.map = map

    def getCost(self, mapPosition):
        return 1

    def isWalkable(self, mapPosition):
        tileType = self.map.getTileType(mapPosition)
        return self.map.isInBounds(mapPosition) and tileType != TileType.NONE and tileType != TileType.WALL

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
        
        neighbors = filter(self.map.isInBounds, neighbors)
        neighbors = filter(self.isWalkable, neighbors)
        return neighbors

class Pathfinder:
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
