# coding: utf-8

import glm
import heapq
import logging

import bot
from botbattles import TransformComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import ActionComponent
from botbattles import MapComponent

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
        self.stopMove = False
        self.finishedMove = False
        self.pathFollower = PathFollower(PathFinder(self.bot.graph))
        self.minSeekDistance = 1.0
        # Rotation
        self.stopRotate = False
        self.finishedRotate = False
        self.autoRotate = False
        self.worldDestinationRotation = None
        self.minAlignDistance = 1.0

    def update(self, input):
        # Movement
        self.finishedMove = True

        if self.stopMove == False:
            linearVelocity = self.move()
            if linearVelocity != glm.vec2(0.0, 0.0):
                self.finishedMove = False

            input.linearVelocityX = linearVelocity.x
            input.linearVelocityY = linearVelocity.y

        # Rotation
        self.finishedRotate = True

        if self.stopRotate == False:
            angularVelocity = self.rotate()
            if angularVelocity != 0.0:
                self.finishedRotate = False

            input.angularVelocity = angularVelocity

    def move(self):
        mapDestinationPosition = self.pathFollower.getWaypoint()
        if mapDestinationPosition == None:
            return glm.vec2(0.0, 0.0)

        worldDestinationPosition = self.bot.map.getWorldPosition(mapDestinationPosition)
        linearVelocity = self.seek(self.bot.transform.position, worldDestinationPosition)

        if glm.length(linearVelocity) == 0.0:
            self.pathFollower.increaseWaypoint()
        return linearVelocity

    def rotate(self):
        if self.autoRotate == True:
            vector = glm.vec2(self.bot.rigidbody.linearVelocity[0], self.bot.rigidbody.linearVelocity[1])
            direction = glm.normalize(vector)
            self.lookAt((direction.x, direction.y))

        if self.worldDestinationRotation == None:
            return 0.0

        angularVelocity = self.align(self.bot.transform.rotation, self.worldDestinationRotation)
        return angularVelocity

    def goTo(self, worldOriginPosition, worldDestinationPosition): # positions
        mapOriginPosition = self.bot.map.getMapPosition(worldOriginPosition)
        mapDestinationPosition = self.bot.map.getMapPosition(worldDestinationPosition)
        self.pathFollower.createPath(mapOriginPosition, mapDestinationPosition)

    def lookAt(self, worldDestinationDirection): # direction
        self.worldDestinationRotation = glm.atan(worldDestinationDirection[1], worldDestinationDirection[0]) * 180.0 / glm.pi()

    def seek(self, worldOriginPosition, worldDestinationPosition): # positions
        vector = glm.vec2(worldDestinationPosition[0] - worldOriginPosition[0], worldDestinationPosition[1] - worldOriginPosition[1])
        if glm.length(vector) <= self.minSeekDistance:
            return glm.vec2(0.0, 0.0)
        
        direction = glm.normalize(vector)
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