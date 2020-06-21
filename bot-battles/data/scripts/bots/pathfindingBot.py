# coding: utf-8

import glm
import heapq
import logging
import random

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

class PathfindingBot(bot.Bot):
    def __init__(self, transformComponent : TransformComponent, rigidbodyComponent : RigidbodyComponent, colliderComponent : ColliderComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
        self.graph = Graph(self.map)
        self.agent = movement.Agent(self)
        self.fsm = decisionMaking.FSM(self)
        self.lastSeenBotEntity = None
        self.lastKnownPosition = None
        self.lastKnownDirection = None

    def tick(self, input : InputComponent):
        if self.action.canPerformAction:
            self.think()
        
        self.fsm.updateCurrentState(input)
        self.agent.update(input)

    def onSeenNewBot(self, input, seenBotEntity):
        logging.info('EVENT: onSeenNewBot')
        self.lastSeenBotEntity = seenBotEntity

    def onSeenLostBot(self, input, seenBotEntity):
        logging.info('EVENT: onSeenLostBot')
        self.lastSeenBotEntity = None

        seenBotInfo = self.sight.getSeenBotInfo(seenBotEntity)
        self.lastKnownPosition = seenBotInfo.transform.position

    def onHitByBullet(self, input, health, direction):
        logging.info('EVENT: onHitByBullet')

        self.lastKnownDirection = (-direction[0], -direction[1])

    def think(self):
        if self.lastSeenBotEntity != None:
            seenBotInfo = self.sight.getSeenBotInfo(self.lastSeenBotEntity)
            distance = glm.distance(glm.vec2(seenBotInfo.transform.position[0], seenBotInfo.transform.position[1]), glm.vec2(self.transform.position[0], self.transform.position[1]))
            colliderRange = self.collider.size[0] / 2.0 + seenBotInfo.collider.size[0] / 2.0

            # Health
            if self.health.currentHP < self.health.maxHP / 2.0 or self.health.currentHP < seenBotInfo.health.currentHP / 2.0:
                if self.health.firstAidBoxHP > 0:
                    if self.fsm.isCurrentState("Heal") == False:
                        self.fsm.changeCurrentState(decisionMaking.Heal())
                else:
                    if self.fsm.isCurrentState("GoToHealthSpawner") == False:
                        farthestHealthSpawner = self.getFarthestHealthSpawner(seenBotInfo.transform.position)
                        worldDestinationPosition = self.map.getWorldPosition(farthestHealthSpawner)
                        self.fsm.changeCurrentState(decisionMaking.GoToHealthSpawner(self.transform.position, worldDestinationPosition))
            # Weapon
            elif self.weapon.currentAmmo > 0:
                if distance <= self.weapon.primaryWeaponRange:
                    if distance >= colliderRange:
                        if self.fsm.isCurrentState("ShootPrimaryWeapon") == False:
                            self.fsm.changeCurrentState(decisionMaking.ShootPrimaryWeapon(self.lastSeenBotEntity))
                    else:
                        if self.fsm.isCurrentState("MoveAwayFromBot") == False:
                            self.fsm.changeCurrentState(decisionMaking.MoveAwayFromBot(self.lastSeenBotEntity))
                else:
                    if self.fsm.isCurrentState("MoveTowardsBot") == False:
                        self.fsm.changeCurrentState(decisionMaking.MoveTowardsBot(self.lastSeenBotEntity))  
            elif self.weapon.ammoBoxAmmo > 0:
                if self.fsm.isCurrentState("Reload") == False:
                    self.fsm.changeCurrentState(decisionMaking.Reload())
            else:
                if distance <= self.weapon.secondaryWeaponRange:
                    if distance >= colliderRange:
                        if self.fsm.isCurrentState("ShootSecondaryWeapon") == False:
                            self.fsm.changeCurrentState(decisionMaking.ShootSecondaryWeapon(self.lastSeenBotEntity))
                    else:
                        if self.fsm.isCurrentState("MoveAwayFromBot") == False:
                            self.fsm.changeCurrentState(decisionMaking.MoveAwayFromBot(self.lastSeenBotEntity))
                else:
                    if self.fsm.isCurrentState("MoveTowardsBot") == False:
                        self.fsm.changeCurrentState(decisionMaking.MoveTowardsBot(self.lastSeenBotEntity))  
        else:
            # Health
            if self.health.currentHP < self.health.maxHP / 2.0:
                if self.health.firstAidBoxHP > 0:
                    if self.fsm.isCurrentState("Heal") == False:
                        self.fsm.changeCurrentState(decisionMaking.Heal())
                else:
                    if self.fsm.isCurrentState("GoToHealthSpawner") == False:
                        closestHealthSpawner = self.getClosestHealthSpawner(self.transform.position)
                        worldDestinationPosition = self.map.getWorldPosition(closestHealthSpawner)
                        self.fsm.changeCurrentState(decisionMaking.GoToHealthSpawner(self.transform.position, worldDestinationPosition))
            # Weapon
            elif self.weapon.maxAmmo == 0 or self.weapon.currentAmmo < self.weapon.maxAmmo:
                if self.weapon.ammoBoxAmmo > 0:
                    if self.fsm.isCurrentState("Reload") == False:
                        self.fsm.changeCurrentState(decisionMaking.Reload())
                else:
                    if self.fsm.isCurrentState("GoToWeaponSpawner") == False:
                        closestWeaponSpawner = self.getClosestWeaponSpawner(self.transform.position)
                        worldDestinationPosition = self.map.getWorldPosition(closestWeaponSpawner)
                        self.fsm.changeCurrentState(decisionMaking.GoToWeaponSpawner(self.transform.position, worldDestinationPosition))            
            # Other
            else:
                if self.lastKnownDirection != None:
                    if self.fsm.isCurrentState("LookAtBullet") == True:
                        if self.agent.finishedRotate == True:
                            self.lastKnownDirection = None
                    else:
                        self.fsm.changeCurrentState(decisionMaking.LookAtBullet(self.lastKnownDirection))
                elif self.lastKnownPosition != None:
                    if self.fsm.isCurrentState("GoToLastKnownPosition") == True:
                        self.lastKnownPosition = None
                    else:
                        self.fsm.changeCurrentState(decisionMaking.GoToLastKnownPosition(self.transform.position, self.lastKnownPosition))
                else:
                    if self.agent.finishedMove == True:
                        randomTile = self.getRandomTile()
                        worldDestinationPosition = self.map.getWorldPosition(randomTile)
                        self.fsm.changeCurrentState(decisionMaking.GoToRandomPosition(self.transform.position, worldDestinationPosition))
                         
    def getClosestWeaponSpawner(self, worldPosition):
        return self.getClosestTileOfType(TileType.WEAPON_SPAWNER, worldPosition)

    def getClosestHealthSpawner(self, worldPosition):
        return self.getClosestTileOfType(TileType.HEALTH_SPAWNER, worldPosition)

    def getClosestTileOfType(self, type, worldPosition):
        tiles = self.graph.getTilesOfType(type)
        closestTile = None
        for tile in tiles:
            if closestTile == None:
                closestTile = tile
                continue

            closestTileWorldPosition = self.map.getWorldPosition(closestTile)
            closestTileDistance = glm.distance(glm.vec2(closestTileWorldPosition[0], closestTileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            tileWorldPosition = self.map.getWorldPosition(tile)
            tileDistance = glm.distance(glm.vec2(tileWorldPosition[0], tileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            if tileDistance < closestTileDistance:
                closestTile = tile
        return closestTile

    def getFarthestWeaponSpawner(self, worldPosition):
        return self.getFarthestTileOfType(TileType.WEAPON_SPAWNER, worldPosition)

    def getFarthestHealthSpawner(self, worldPosition):
        return self.getFarthestTileOfType(TileType.HEALTH_SPAWNER, worldPosition)

    def getFarthestTileOfType(self, type, worldPosition):
        tiles = self.graph.getTilesOfType(type)
        farthestTile = None
        for tile in tiles:
            if farthestTile == None:
                farthestTile = tile
                continue

            farthestTileWorldPosition = self.map.getWorldPosition(farthestTile)
            farthestTileDistance = glm.distance(glm.vec2(farthestTileWorldPosition[0], farthestTileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            tileWorldPosition = self.map.getWorldPosition(tile)
            tileDistance = glm.distance(glm.vec2(tileWorldPosition[0], tileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            if tileDistance > farthestTileDistance:
                farthestTile = tile
        return farthestTile

    def getClosestCover(self, worldPosition):
        tiles = self.graph.getWalkableTiles()
        closestTile = None
        for tile in tiles:
            tileWorldPosition = self.map.getWorldPosition(tile)
            if self.map.isVisible(worldPosition, tileWorldPosition) == False:
                continue

            if closestTile == None:
                closestTile = tile
                continue

            closestTileWorldPosition = self.map.getWorldPosition(closestTile)
            closestTileDistance = glm.distance(glm.vec2(closestTileWorldPosition[0], closestTileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            tileDistance = glm.distance(glm.vec2(tileWorldPosition[0], tileWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            if tileDistance < closestTileDistance:
                closestTile = tile
        return closestTile

    def getRandomTile(self):
        x = random.randint(0, self.map.tileCount[0] - 1)
        y = random.randint(0, self.map.tileCount[1] - 1)
        randomTile = (x, y)
        return self.getClosestWalkable(randomTile)

    def getClosestWalkable(self, mapPosition):
        if self.graph.isWalkable(mapPosition) == True:
            return mapPosition

        tiles = self.graph.getWalkableTiles()
        closestTile = None
        for tile in tiles:
            if closestTile == None:
                closestTile = tile
                continue

            closestTileDistance = glm.distance(glm.vec2(closestTile[0], closestTile[1]), glm.vec2(mapPosition[0], mapPosition[1]))
            tileDistance = glm.distance(glm.vec2(tile[0], tile[1]), glm.vec2(mapPosition[0], mapPosition[1]))
            if tileDistance < closestTileDistance:
                closestTile = tile
        return closestTile

class Graph:
    def __init__(self, map : MapComponent):
        self.map = map

    def getCost(self, mapPosition):
        return 1

    def isInBounds(self, mapPosition):
        return ((0 <= mapPosition[0] and mapPosition[0] < self.map.tileCount[0]) and (0 <= mapPosition[1] and mapPosition[1] < self.map.tileCount[1]))

    def isWalkable(self, mapPosition):
        return self.isInBounds(mapPosition) and self.map.getTileType(mapPosition) != TileType.NONE and self.map.getTileType(mapPosition) != TileType.WALL

    def getNeighborTiles(self, mapPosition, diagonals):
        north = (mapPosition[0], mapPosition[1] + 1)
        south = (mapPosition[0], mapPosition[1] - 1)
        east = (mapPosition[0] + 1, mapPosition[1])
        west = (mapPosition[0] - 1, mapPosition[1])
        tiles = [north, south, east, west]

        if diagonals == True:
            northEast = (mapPosition[0] + 1, mapPosition[1] + 1)
            northWest = (mapPosition[0] - 1, mapPosition[1] + 1)
            southEast = (mapPosition[0] + 1, mapPosition[1] - 1)
            southWest = (mapPosition[0] - 1, mapPosition[1] - 1)
            tiles.append(northEast)
            tiles.append(northWest)
            tiles.append(southEast)
            tiles.append(southWest)
        
        tiles = filter(self.isWalkable, tiles)
        return tiles

    def getWalkableTiles(self):
        tiles = []

        for i in range(0, self.map.tileCount[0]):
            for j in range(0, self.map.tileCount[1]):
                tile = (i, j)
                if self.isWalkable(tile) == True:
                    tiles.append(tile)
        return tiles

    def getTilesOfType(self, type):
        tiles = []

        for i in range(0, self.map.tileCount[0]):
            for j in range(0, self.map.tileCount[1]):
                tile = (i, j)
                tileType = self.map.getTileType(tile)
                if tileType == type:
                    tiles.append(tile)
        return tiles