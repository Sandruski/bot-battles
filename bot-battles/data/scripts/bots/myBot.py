# coding: utf-8

#import numpy as np
import glm
import logging
import bot
import heapq

from botbattles import InputComponent

class MyBot(bot.Bot):

    wallHit = False

    def tick(self, input : InputComponent):
        if self.wallHit == False:
            input.linearVelocityX = 0#int(self.map.getTile(1, 1).tileType) * 100
            input.linearVelocityY = 0
        input.angularVelocity = 0
        input.shootSecondaryWeapon()
        worldPosition = self.map.getWorldPosition((0,0))
        logging.info('%f %f', worldPosition[0], worldPosition[1])
        #a_star_search(self.map, (1,1),(5,5))

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

def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = came_from[current]
    path.append(start) # optional
    path.reverse() # optional
    return path

def heuristic(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2)

def neighbors(map, id, diagonals):
    results = []
    north = (id[0], id[1] + 1)
    
    [(x+1, y), (x, y-1), (x-1, y), (x, y+1)]
    if (x + y) % 2 == 0: results.reverse() # aesthetics
    results = filter(self.in_bounds, results)
    results = filter(self.passable, results)
    return results

def cost(from_node, to_node):
    return self.weights.get(to_node, 1)

def a_star_search(graph, start, goal):
    frontier = PriorityQueue()
    frontier.put(start, 0)
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0
    
    while not frontier.empty():
        current = frontier.get()
        
        if current == goal:
            break
        
        for next in graph.getNeighbors(current, True):
            new_cost = cost_so_far[current] + graph.cost(current, next)
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost + heuristic(goal, next)
                frontier.put(next, priority)
                came_from[next] = current
    
    return came_from, cost_so_far
