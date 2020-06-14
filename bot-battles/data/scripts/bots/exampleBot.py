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

class ExampleBot(bot.Bot):
    def __init__(self, transformComponent : TransformComponent, colliderComponent : ColliderComponent, rigidbodyComponent : RigidbodyComponent, weaponComponent : WeaponComponent, healthComponent : HealthComponent, sightComponent : SightComponent, actionComponent : ActionComponent, mapComponent : MapComponent):
        super().__init__(transformComponent, colliderComponent, rigidbodyComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
        self.graph = Graph(self.map)
        self.agent = movement.Agent(self)
        self.fsm = decisionMaking.FSM(self)
        self.lastSeenBotEntity = None
        self.lastKnownPosition = None
        self.lastKnownDirection = None

        closestCenter = self.getClosestCenter()
        self.closestCenterWorldPosition = self.map.getWorldPosition(closestCenter)

    def tick(self, input : InputComponent):
        if self.lastSeenBotEntity != None:
            seenBotInfo = self.sight.getSeenBotInfo(self.lastSeenBotEntity)
            distance = glm.distance(glm.vec2(seenBotInfo.transform.position[0], seenBotInfo.transform.position[1]),  glm.vec2(self.transform.position[0], self.transform.position[1]))
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
                        if self.fsm.isCurrentState("GoBackward") == False:
                            worldDestinationDirection = (-self.transform.direction[0], -self.transform.direction[1])
                            self.fsm.changeCurrentState(decisionMaking.GoBackward(worldDestinationDirection))
                else:
                    if self.fsm.isCurrentState("GoForward") == False:
                        worldDestinationDirection = self.transform.direction
                        self.fsm.changeCurrentState(decisionMaking.GoForward(worldDestinationDirection))  
            elif self.weapon.ammoBoxAmmo > 0:
                if self.fsm.isCurrentState("Reload") == False:
                    self.fsm.changeCurrentState(decisionMaking.Reload())
            else:
                if distance <= self.weapon.secondaryWeaponRange:
                    if distance >= colliderRange:
                        if self.fsm.isCurrentState("ShootSecondaryWeapon") == False:
                            self.fsm.changeCurrentState(decisionMaking.ShootSecondaryWeapon(self.lastSeenBotEntity))
                    else:
                        if self.fsm.isCurrentState("GoBackward") == False:
                            worldDestinationDirection = (-self.transform.direction[0], -self.transform.direction[1])
                            self.fsm.changeCurrentState(decisionMaking.GoBackward(worldDestinationDirection))
                else:
                    if self.fsm.isCurrentState("GoForward") == False:
                        worldDestinationDirection = self.transform.direction
                        self.fsm.changeCurrentState(decisionMaking.GoForward(worldDestinationDirection))  
        else:
            # Health
            if self.health.currentHP < self.health.maxHP / 2.0:
                if self.health.firstAidBoxHP > 0:
                    if self.fsm.isCurrentState("Heal") == False:
                        self.fsm.changeCurrentState(decisionMaking.Heal())
                else:
                    if self.fsm.isCurrentState("GoToHealthSpawner") == False:
                        closestHealthSpawner = self.getClosestHealthSpawner()
                        worldDestinationPosition = self.map.getWorldPosition(closestHealthSpawner)
                        self.fsm.changeCurrentState(decisionMaking.GoToHealthSpawner(self.transform.position, worldDestinationPosition))
            # Weapon
            elif self.weapon.currentAmmo < self.weapon.maxAmmo:
                if self.weapon.ammoBoxAmmo > 0:
                    if self.fsm.isCurrentState("Reload") == False:
                        self.fsm.changeCurrentState(decisionMaking.Reload())
                else:
                    if self.fsm.isCurrentState("GoToWeaponSpawner") == False:
                        closestWeaponSpawner = self.getClosestWeaponSpawner()
                        worldDestinationPosition = self.map.getWorldPosition(closestWeaponSpawner)
                        self.fsm.changeCurrentState(decisionMaking.GoToWeaponSpawner(self.transform.position, worldDestinationPosition))
            # ...
            elif self.lastKnownDirection != None:
                if self.fsm.isCurrentState("LookAtBullet") == True:
                    if self.agent.finishedRotate == True:
                        self.lastKnownDirection = None
                else:
                    self.fsm.changeCurrentState(decisionMaking.LookAtBullet(self.lastKnownDirection))
            elif self.lastKnownPosition != None:
                if self.fsm.isCurrentState("GoToLastKnownPosition") == True:
                    if self.agent.finishedMove == True:
                        self.lastKnownPosition = None
                else:
                    self.fsm.changeCurrentState(decisionMaking.GoToLastKnownPosition(self.transform.position, self.lastKnownPosition))
            else:
                if glm.distance(glm.vec2(self.closestCenterWorldPosition[0], self.closestCenterWorldPosition[1]),  glm.vec2(self.transform.position[0], self.transform.position[1])) <= self.agent.minSeekDistance:
                    if self.fsm.isCurrentState("Rotate") == False:
                        self.fsm.changeCurrentState(decisionMaking.Rotate(self.rigidbody.maxAngularVelocity / 2.0))
                else:
                    if self.fsm.isCurrentState("GoToCenterMap") == False:
                        worldDestinationPosition = self.closestCenterWorldPosition
                        self.fsm.changeCurrentState(decisionMaking.GoToCenterMap(self.transform.position, worldDestinationPosition))

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
        logging.info('lkp is %f %f', self.lastKnownPosition[0], self.lastKnownPosition[1])

    def onHitByBullet(self, input, health, direction):
        logging.info('EVENT: onHitByBullet')

        self.lastKnownDirection = (-direction[0], -direction[1])

    def getClosestWeaponSpawner(self):
        weaponSpawnerTiles = self.graph.getTilesOfType(TileType.WEAPON_SPAWNER)
        closestWeaponSpawnerTile = None
        for weaponSpawnerTile in weaponSpawnerTiles:
            if closestWeaponSpawnerTile == None:
                closestWeaponSpawnerTile = weaponSpawnerTile
                continue

            closestWeaponSpawnerWorldPosition = self.map.getWorldPosition(closestWeaponSpawnerTile)
            closestWeaponSpawnerDistance = glm.distance(glm.vec2(closestWeaponSpawnerWorldPosition[0], closestWeaponSpawnerWorldPosition[1]), glm.vec2(self.transform.position[0], self.transform.position[1]))
            weaponSpawnerWorldPosition = self.map.getWorldPosition(weaponSpawnerTile)
            weaponSpawnerDistance = glm.distance(glm.vec2(weaponSpawnerWorldPosition[0], weaponSpawnerWorldPosition[1]), glm.vec2(self.transform.position[0], self.transform.position[1]))
            if weaponSpawnerDistance < closestWeaponSpawnerDistance:
                closestWeaponSpawnerTile = weaponSpawnerTile     
        return closestWeaponSpawnerTile

    def getClosestHealthSpawner(self):
        healthSpawnerTiles = self.graph.getTilesOfType(TileType.HEALTH_SPAWNER)
        closestHealthSpawnerTile = None
        for healthSpawnerTile in healthSpawnerTiles:
            if closestHealthSpawnerTile == None:
                closestHealthSpawnerTile = healthSpawnerTile
                continue

            closestHealthSpawnerWorldPosition = self.map.getWorldPosition(closestHealthSpawnerTile)
            closestHealthSpawnerDistance = glm.distance(glm.vec2(closestHealthSpawnerWorldPosition[0], closestHealthSpawnerWorldPosition[1]), glm.vec2(self.transform.position[0], self.transform.position[1]))
            healthSpawnerWorldPosition = self.map.getWorldPosition(healthSpawnerTile)
            healthSpawnerDistance = glm.distance(glm.vec2(healthSpawnerWorldPosition[0], healthSpawnerWorldPosition[1]), glm.vec2(self.transform.position[0], self.transform.position[1]))
            if healthSpawnerDistance < closestHealthSpawnerDistance:
                closestHealthSpawnerTile = healthSpawnerTile
        return closestHealthSpawnerTile

    def getFarthestHealthSpawner(self, worldPosition):
        healthSpawnerTiles = self.graph.getTilesOfType(TileType.HEALTH_SPAWNER)
        farthestHealthSpawnerTile = None
        for healthSpawnerTile in healthSpawnerTiles:
            if farthestHealthSpawnerTile == None:
                farthestHealthSpawnerTile = healthSpawnerTile
                continue

            farthestHealthSpawnerWorldPosition = self.map.getWorldPosition(farthestHealthSpawnerTile)
            farthestHealthSpawnerDistance = glm.distance(glm.vec2(farthestHealthSpawnerWorldPosition[0], farthestHealthSpawnerWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            healthSpawnerWorldPosition = self.map.getWorldPosition(healthSpawnerTile)
            healthSpawnerDistance = glm.distance(glm.vec2(healthSpawnerWorldPosition[0], healthSpawnerWorldPosition[1]), glm.vec2(worldPosition[0], worldPosition[1]))
            if healthSpawnerDistance > farthestHealthSpawnerDistance:
                farthestHealthSpawnerTile = healthSpawnerTile
        return farthestHealthSpawnerTile

    def getClosestCenter(self):
        center = (self.map.tileCount[0] // 2, self.map.tileCount[1] // 2)
        if self.graph.isWalkable(center) == True:
            return center

        closestCenterTile = None
        for i in range(0, self.map.tileCount[0]):
            for j in range(0, self.map.tileCount[1]):
                tile = (i, j)

                if self.graph.isWalkable(tile) == False:
                    continue

                if closestCenterTile == None:
                    closestCenterTile = tile
                    continue

                closestCenterDistance = glm.distance(glm.vec2(closestCenterTile[0], closestCenterTile[1]), glm.vec2(center[0], center[1]))
                tileDistance = glm.distance(glm.vec2(tile[0], tile[1]), glm.vec2(center[0], center[1]))
                if tileDistance < closestCenterDistance:
                    closestCenterTile = tile
        return closestCenterTile

class Graph:
    def __init__(self, map : MapComponent):
        self.map = map

    def getCost(self, mapPosition):
        return 1

    def isInBounds(self, mapPosition):
        return ((0 <= mapPosition[0] and mapPosition[0] < self.map.tileCount[0]) and (0 <= mapPosition[1] and mapPosition[1] < self.map.tileCount[1]))

    def isWalkable(self, mapPosition):
        return self.isInBounds(mapPosition) and self.map.getTileType(mapPosition) != TileType.NONE and self.map.getTileType(mapPosition) != TileType.WALL

    def getNeighbors(self, mapPosition, diagonals):
        north = (mapPosition[0], mapPosition[1] + 1)
        south = (mapPosition[0], mapPosition[1] - 1)
        east = (mapPosition[0] + 1, mapPosition[1])
        west = (mapPosition[0] - 1, mapPosition[1])
        neighbors = [north, south, east, west]

        if diagonals == True:
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