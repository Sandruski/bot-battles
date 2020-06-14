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

class PriorityQueue:
    def __init__(self):
        self.elements = []
    
    def empty(self):
        return len(self.elements) == 0
    
    def put(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self):
        return heapq.heappop(self.elements)[1]

class PathFinder:
    def __init__(self, graph):
        self.graph = graph

    def reconstructPath(self, origin, destination, cameFrom):
       logging.info('origin %i %i, destination %i %i', origin[0], origin[1], destination[0], destination[1])
       current = destination
       path = []
       while current != origin:
           path.append(current)
           current = cameFrom[current]
           if current == None:
               logging.info('current is none')
           logging.info('current %i %i', current[0], current[1])
       path.append(origin)
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
        if self.path == None:
            return None

        if self.index >= len(self.path):
            return None

        return self.path[self.index]

    def increaseWaypoint(self):
        self.index += 1

class Agent:
    def __init__(self, bot):
        self.bot = bot
        # Movement
        self.stopMove = False
        self.finishedMove = False
        self.followPath = False
        self.pathFollower = PathFollower(PathFinder(self.bot.graph))
        self.worldDestinationDirection = None
        self.minSeekDistance = 1.0
        # Rotation
        self.stopRotate = False
        self.finishedRotate = False
        self.autoRotate = False
        self.useAngularVelocity = False
        self.worldDestinationRotation = None
        self.angularVelocity = None
        self.minAlignDistance = 1.5

    def update(self, input):
        # Movement
        if self.stopMove == False:
            linearVelocity = self.move()

            input.linearVelocityX = linearVelocity.x
            input.linearVelocityY = linearVelocity.y
        else:
            input.linearVelocityX = 0.0
            input.linearVelocityY = 0.0

        # Rotation
        if self.stopRotate == False:
            angularVelocity = self.rotate()
            if angularVelocity == 0.0:
                self.finishedRotate = True
            input.angularVelocity = angularVelocity
        else:
            input.angularVelocity = 0.0

    def move(self):
        linearVelocity = glm.vec2(0.0, 0.0)
        if self.followPath == True:
            mapDestinationPosition = self.pathFollower.getWaypoint()
            if mapDestinationPosition == None:
                self.finishedMove = True
                return glm.vec2(0.0, 0.0)

            worldDestinationPosition = self.bot.map.getWorldPosition(mapDestinationPosition)
            linearVelocity = self.seekPosition(self.bot.transform.position, worldDestinationPosition)

            if glm.length(linearVelocity) == 0.0:
                if self.pathFollower.increaseWaypoint() == False:
                    self.finishedMove = True
        else:
            if self.worldDestinationDirection == None:
                self.finishedMove = True
                return glm.vec2(0.0, 0.0)

            linearVelocity = self.seekDirection(self.worldDestinationDirection)
            if glm.length(linearVelocity) == 0.0:
                self.finishedMove = True
        return linearVelocity

    def rotate(self):
        if self.autoRotate == True:
            vector = glm.vec2(self.bot.rigidbody.linearVelocity[0], self.bot.rigidbody.linearVelocity[1])
            direction = glm.vec2(0.0, 0.0)
            if glm.length(vector) > 0.0:
                direction = glm.normalize(vector)
            self.lookAt((direction.x, direction.y))

        angularVelocity = 0.0
        if self.useAngularVelocity == True:
            if self.angularVelocity == None:
                return 0.0

            angularVelocity = self.angularVelocity
        else:
            if self.worldDestinationRotation == None:
                return 0.0

            angularVelocity = self.align(self.bot.transform.rotation, self.worldDestinationRotation)
        return angularVelocity

    def goToPosition(self, worldOriginPosition, worldDestinationPosition): # positions
        mapOriginPosition = self.bot.map.getMapPosition(worldOriginPosition)
        mapDestinationPosition = self.bot.map.getMapPosition(worldDestinationPosition)
        if mapOriginPosition != mapDestinationPosition:
            self.finishedMove = False

        self.pathFollower.createPath(mapOriginPosition, mapDestinationPosition)
        self.followPath = True

    def goToDirection(self, worldDestinationDirection): # direction
        self.worldDestinationDirection = worldDestinationDirection
        self.finishedMove = False

        self.followPath = False

    def lookAt(self, worldDestinationDirection): # direction
        newWorldDestinationRotation = glm.atan(worldDestinationDirection[1], worldDestinationDirection[0]) * 180.0 / glm.pi()
        if newWorldDestinationRotation != self.worldDestinationRotation:
            self.finishedRotate = False
        
        self.worldDestinationRotation = newWorldDestinationRotation

    def seekPosition(self, worldOriginPosition, worldDestinationPosition): # positions
        vector = glm.vec2(worldDestinationPosition[0] - worldOriginPosition[0], worldDestinationPosition[1] - worldOriginPosition[1])
        if glm.length(vector) <= self.minSeekDistance:
            return glm.vec2(0.0, 0.0)
        
        direction = glm.vec2(0.0, 0.0)
        if glm.length(vector) > 0.0:
            direction = glm.normalize(vector)
        acceleration = direction * self.bot.rigidbody.maxLinearVelocity
        return acceleration

    def seekDirection(self, worldDestinationDirection): # direction
        direction = glm.vec2(worldDestinationDirection[0], worldDestinationDirection[1])
        acceleration = direction * self.bot.rigidbody.maxLinearVelocity
        return acceleration

    def align(self, worldOriginRotation, worldDestinationRotation): # rotations
        angle = worldDestinationRotation - worldOriginRotation
        angle = (angle + 180.0) % 360.0 - 180.0
        absAngle = glm.abs(angle)
        if absAngle <= self.minAlignDistance:
            return 0.0

        direction = angle / absAngle
        angularAcceleration = direction * self.bot.rigidbody.maxAngularVelocity
        return angularAcceleration