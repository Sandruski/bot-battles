# coding: utf-8

import exampleBot

class EasyBot(exampleBot.ExampleBot):
    def __init__(self, transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent):
        super().__init__(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent, 
                         actionDelay=1.0, reactionProbability=0.7, canPickUpObjects=False, canTakeCover=False)