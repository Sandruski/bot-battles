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
        current = destination
        path = []
        while current != origin:
            path.append(current)
            current = cameFrom[current]
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
        self.linearVelocity = None
        self.stopMove = False
        self.finishedMove = False
        self.followPath = False
        self.pathFollower = PathFollower(PathFinder(self.bot.graph))
        self.worldDestinationDirection = None
        self.minSeekDistance = 1.0
        # Rotation
        self.angularVelocity = None
        self.stopRotate = False
        self.finishedRotate = False
        self.autoRotate = False
        self.useAngularVelocity = False
        self.worldDestinationRotation = None
        self.angularVelocity = None
        self.minAlignDistance = 1.0

    def update(self, input):
        # Movement
        if self.stopMove == False:
            self.linearVelocity = self.move()
        else:
            self.linearVelocity = glm.vec2(0.0, 0.0)
        if glm.length(self.linearVelocity) == 0.0:
            self.finishedMove = True

        # Rotation
        if self.stopRotate == False:
            self.angularVelocity = self.rotate()
        else:
            self.angularVelocity = 0.0
        if self.angularVelocity == 0.0:
            self.finishedRotate = True
        
        input.linearVelocityX = self.linearVelocity.x
        input.linearVelocityY = self.linearVelocity.y
        input.angularVelocity = self.angularVelocity

    def move(self):
        linearVelocity = glm.vec2(0.0, 0.0)
        if self.followPath == True:
            mapDestinationPosition = self.pathFollower.getWaypoint()
            if mapDestinationPosition == None:
                return glm.vec2(0.0, 0.0)

            worldDestinationPosition = self.bot.map.getWorldPosition(mapDestinationPosition)
            vector = glm.vec2(worldDestinationPosition[0] - self.bot.transform.position[0], worldDestinationPosition[1] - self.bot.transform.position[1])
            if glm.length(vector) <= self.minSeekDistance:
                self.pathFollower.increaseWaypoint()

            mapDestinationPosition = self.pathFollower.getWaypoint()
            if mapDestinationPosition == None:
                return glm.vec2(0.0, 0.0)

            worldDestinationPosition = self.bot.map.getWorldPosition(mapDestinationPosition)
            linearVelocity = self.seekPosition(self.bot.transform.position, worldDestinationPosition)
        else:
            if self.worldDestinationDirection == None:
                return glm.vec2(0.0, 0.0)

            linearVelocity = self.seekDirection(self.worldDestinationDirection)
        return linearVelocity

    def rotate(self):
        if self.autoRotate == True:
            if glm.length(self.linearVelocity) > 0.0:
                direction = glm.normalize(self.linearVelocity)
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