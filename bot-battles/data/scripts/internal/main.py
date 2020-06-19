# coding: utf-8

import sys
import os
import datetime
import io
import inspect
import logging

import bot
import botbattles

stream = None
myBot = None

def init(scriptName, player, transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent):
    for name, obj in inspect.getmembers(sys.modules[scriptName]):
        if inspect.isclass(obj) and issubclass(obj, bot.Bot):
            global myBot
            myBot = obj(transformComponent, rigidbodyComponent, colliderComponent, weaponComponent, healthComponent, sightComponent, actionComponent, mapComponent)
            break

    logger = logging.getLogger('')

    logger.setLevel(logging.NOTSET)

    global stream
    stream = io.StringIO()
    streamHandler = logging.StreamHandler(stream)
    now = datetime.datetime.now()
    nowText = now.strftime("%d.%m.%Y-%H.%M.%S")
    playerNumber = player + 1
    fileHandler = logging.FileHandler('%s_%s_P%u.log' % (nowText, myBot.__class__.__name__, playerNumber))
    formatter = logging.Formatter('[%(levelname)s] %(message)s')
    streamHandler.setFormatter(formatter)
    fileHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)
    logger.addHandler(fileHandler)

def tick(input): # TODO: uncomment
    #try:
    myBot.tick(input)
    #except:
        #pass

def onSeenNewBot(input, seenBotEntity):
    try:
        myBot.onSeenNewBot(input, seenBotEntity)
    except:
        pass

def onSeenNewWeapon(input, seenWeaponEntity):
    try:
        myBot.onSeenNewWeapon(input, seenWeaponEntity)
    except:
        pass

def onSeenNewHealth(input, seenHealthEntity):
    try:
        myBot.onSeenNewHealth(input, seenHealthEntity)
    except:
        pass

def onSeenLostBot(input, seenBotEntity):
    try:
        myBot.onSeenLostBot(input, seenBotEntity)
    except:
        pass

def onSeenLostWeapon(input, seenWeaponEntity):
    try:
        myBot.onSeenLostWeapon(input, seenWeaponEntity)
    except:
        pass

def onSeenLostHealth(input, seenHealthEntity):
    try:
        myBot.onSeenLostHealth(input, seenHealthEntity)
    except:
        pass

def onWeaponPickedUp(input):
    try:
        myBot.onWeaponPickedUp(input)
    except:
        pass

def onHealthPickedUp(input):
    try:
        myBot.onHealthPickedUp(input)
    except:
        pass

def onBulletHit(input):
    try:
        myBot.onBulletHit(input)
    except:
        pass

def onBulletMiss(input):
    try:
        myBot.onBulletMiss(input)
    except:
        pass

def onReloaded(input, ammo):
    try:
        myBot.onReloaded(input, ammo)
    except:
        pass

def onHitByBullet(input, health, direction):
    try:
        myBot.onHitByBullet(input, health, direction)
    except:
        pass

def onHealed(input, health):
    try:
        myBot.onHealed(input, health)
    except:
        pass

def onHitWall(input, collision):
    try:
        myBot.onHitWall(input, collision)
    except:
        pass

def log():
    content = stream.getvalue()
    for line in content.splitlines(False):
        botbattles.log('\n%s' % line)
    stream.seek(0)
    stream.truncate(0)