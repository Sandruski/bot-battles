# coding: utf-8

import exampleBot

class HardBot(exampleBot.ExampleBot):
    def __init__(self, transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent):
        super().__init__(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent, 
                         shootDelay=0.0, seeDelay=0.0, aimOffset=0.0, hitReaction=1.0, canPickUpObjects=True, canTakeCover=True)