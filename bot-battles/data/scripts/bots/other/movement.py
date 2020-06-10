# coding: utf-8

import glm
import heapq

import bot
from botbattles import TransformComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
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
        self.lookAtMovement = False
        self.worldDestination = 0
        self.minAlignDistance = 5.0

    def move(self, input):
        # Movement
        mapDestination = self.pathFollower.getWaypoint()
        worldDestination = self.bot.map.getWorldPosition(mapDestination)
        linearVelocity = self.seek(self.bot.transform.position, worldDestination)
        if glm.length(linearVelocity) == 0.0:
            self.pathFollower.increaseWaypoint()

        # Rotation
        if self.lookAtMovement:
            direction = glm.normalize(linearVelocity)
            self.lookAt((direction.x, direction.y))
        angularVelocity = self.align(self.bot.transform.rotation, self.worldDestination)

        input.linearVelocityX = linearVelocity.x
        input.linearVelocityY = linearVelocity.y
        input.angularVelocity = angularVelocity

    def goTo(self, worldOriginPosition, worldDestinationPosition): # positions
        mapOriginPosition = self.bot.map.getMapPosition(worldOriginPosition)
        mapDestinationPosition = self.bot.map.getMapPosition(worldDestinationPosition)
        self.pathFollower.createPath(mapOriginPosition, mapDestinationPosition)

    def lookAt(self, worldDestinationDirection): # direction
        self.worldDestination = glm.atan(worldDestinationDirection[1], worldDestinationDirection[0]) * 180.0 / glm.pi()

    def seek(self, worldOrigin, worldDestination):
        vector = glm.vec2(worldDestination[0] - worldOrigin[0], worldDestination[1] - worldOrigin[1])
        if glm.length(vector) <= self.minSeekDistance:
            return glm.vec2(0.0, 0.0)
        
        direction = glm.normalize(vector)
        acceleration = direction * self.bot.rigidbody.maxLinearVelocity
        return acceleration

    def align(self, worldOrigin, worldDestination):
        angle = worldDestination - worldOrigin
        angle = (angle + 180.0) % 360.0 - 180.0
        absAngle = glm.abs(angle)
        if absAngle <= self.minAlignDistance:
            return 0.0

        direction = angle / absAngle
        angularAcceleration = direction * self.bot.rigidbody.maxAngularVelocity
        return angularAcceleration