# coding: utf-8

import balancedBot
from botbattles import InputComponent
from botbattles import TransformComponent
from botbattles import ColliderComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import SeenBotInfo
from botbattles import SeenWeaponInfo
from botbattles import SeenHealthInfo
from botbattles import ActionComponent
from botbattles import MapComponent
from botbattles import TileType
from botbattles import CollisionEvent

class EasyBot(balancedBot.BalancedBot):
    def __init__(self, 
                 transformComponent : TransformComponent, 
                 rigidbodyComponent : RigidbodyComponent, 
                 colliderComponent : ColliderComponent, 
                 weaponComponent : WeaponComponent, 
                 healthComponent : HealthComponent, 
                 sightComponent : SightComponent, 
                 actionComponent : ActionComponent, 
                 mapComponent : MapComponent):
        super().__init__(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent, 
                         shootDelay=1.0, seeDelay=0.5, aimOffset=20.0, hitReaction=0.5, canPickUpObjects=False, canTakeCover=False)