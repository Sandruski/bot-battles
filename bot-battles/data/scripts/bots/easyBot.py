# coding: utf-8

import balancedBot

class EasyBot(balancedBot.BalancedBot):
    def __init__(self, transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent):
        super().__init__(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent, 
                         shootDelay=1.0, seeDelay=0.5, aimOffset=20.0, hitReaction=0.5, canPickUpObjects=False, canTakeCover=False)