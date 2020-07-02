# coding: utf-8

import logging

import bot
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

class ExampleBot(bot.Bot):
    def tick(self, input : InputComponent):
        # TODO
        logging.info('tick')