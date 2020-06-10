# coding: utf-8

import logging

import bot
from botbattles import TransformComponent
from botbattles import RigidbodyComponent
from botbattles import WeaponComponent
from botbattles import HealthComponent
from botbattles import SightComponent
from botbattles import MapComponent

class State:
    def __init__(self):
        pass

    def enter(self):
        pass

    def update(self):
        pass

    def exit(self):
        pass

class GoTo(State):
    def __init__(self, origin, destination):
        self.origin = origin
        self.destination = destination

    def enter(self, bot):
        pass

    def update(self, bot):
        pass

    def exit(self, bot):
        pass

class FSM:
    def __init__(self, bot, initialState):
        self.bot = bot
        self.currentState = initialState

    def changeCurrentState(newState):
        if self.currentState != None and self.bot != None:
            self.currentState.exit(self.bot)

        self.currentState = newState

        if self.currentState != None and self.bot != None:
            self.currentState.enter(self.bot)

    def updateCurrentState():
        if self.currentState != None and self.bot != None:
            self.currentState.update(self.bot)