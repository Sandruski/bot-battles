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
        self.agent = Agent(self)
        self.fsm = FSM(self, None)
        self.calculatePath = True

    def tick(self, input : InputComponent):
        if self.calculatePath:
            worldDestination = self.map.getWorldPosition((5, 4))
            self.agent.goTo(self.transform.position, worldDestination)
            self.calculatePath = False
            
        self.agent.move(input)

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

    def getPath(self, origin, destination):
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
            
            for next in self.graph.getNeighbors(current, False):
                newCost = costSoFar[current] + self.graph.getCost(next)
                if next not in costSoFar or newCost < costSoFar[next]:
                    costSoFar[next] = newCost
                    priority = newCost + self.calculateHeuristic(destination, next)
                    frontier.put(next, priority)
                    cameFrom[next] = current
        
        return cameFrom

class PathFollower:
    def __init__(self, pathFinder):
        self.pathFinder = pathFinder
        self.path = None
        self.index = 0

    def createPath(self, mapOrigin, mapDestination):
        cameFrom = self.pathFinder.getPath(mapOrigin, mapDestination)
        self.path = self.pathFinder.reconstructPath(mapOrigin, mapDestination, cameFrom)
        self.index = 0

    def getWaypoint(self):
        if self.index >= len(self.path):
            return None

        return self.path[self.index]

    def increaseWaypoint(self):
        if self.index >= len(self.path) - 1:
            return False

        self.index += 1
        return True

class Agent:
    def __init__(self, bot):
        self.bot = bot

        # Movement
        self.pathFollower = PathFollower(PathFinder(self.bot.graph))
        self.minSeekDistance = 1.0
        # Rotation
        self.worldDestination = 0
        self.minAlignDistance = 5.0

    def move(self, input):
        # Movement
        mapDestination = self.pathFollower.getWaypoint()
        worldDestination = self.bot.map.getWorldPosition(mapDestination)
        linearVelocity = self.seek(input, self.bot.transform.position, worldDestination)
        if glm.length(linearVelocity) == 0.0:
            self.pathFollower.increaseWaypoint()
        # Rotation
        angularVelocity = self.align(input, self.bot.transform.rotation, self.worldDestination)

        input.linearVelocityX = linearVelocity.x
        input.linearVelocityY = linearVelocity.y
        input.angularVelocity = angularVelocity

    def goTo(self, worldOrigin, worldDestination):
        mapOrigin = self.bot.map.getMapPosition(worldOrigin)
        mapDestination = self.bot.map.getMapPosition(worldDestination)
        self.pathFollower.createPath(mapOrigin, mapDestination)

    def lookAt(self, worldOrigin, worldDestination):
        worldVector = glm.vec2(worldDestination[0] - worldOrigin[0], worldDestination[1] - worldOrigin[1])
        worldDirection = glm.normalize(vector)
        self.worldDestination = glm.atan(worldDirection[1], worldDirection[0]) * 180.0 / glm.pi()

    def seek(self, input, worldOrigin, worldDestination):
        vector = glm.vec2(worldDestination[0] - worldOrigin[0], worldDestination[1] - worldOrigin[1])
        if glm.length(vector) <= self.minSeekDistance:
            return glm.vec2(0.0, 0.0)
        
        direction = glm.normalize(vector)
        acceleration = direction * input.maxLinearVelocity
        return acceleration

    def align(self, input, worldOrigin, worldDestination):
        angle = worldDestination - worldOrigin
        angle = (angle + 180.0) % 360.0 - 180.0
        absAngle = glm.abs(angle)
        if absAngle <= self.minAlignDistance:
            return 0.0

        direction = angle / absAngle
        angularAcceleration = direction * input.maxAngularVelocity
        return angularAcceleration

class State:
    def __init__(self):
        pass

    def enter(self):
        pass

    def update(self):
        pass

    def exit(self):
        pass

class GoTo(State):
    def __init__(self, origin, destination):
        self.origin = origin
        self.destination = destination

    def enter(self, bot):
        pass

    def update(self, bot):
        pass

    def exit(self, bot):
        pass

class FSM:
    def __init__(self, bot, initialState):
        self.bot = bot
        self.currentState = initialState

    def changeCurrentState(newState):
        if self.currentState != None and self.bot != None:
            self.currentState.exit(self.bot)

        self.currentState = newState

        if self.currentState != None and self.bot != None:
            self.currentState.enter(self.bot)

    def updateCurrentState():
        if self.currentState != None and self.bot != None:
            self.currentState.update(self.bot)