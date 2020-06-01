# coding: utf-8

import sys
import inspect
import logging
import io

import bot
import botbattles

stream = None
myBot = None

def init(scriptName, transformComponent, rigidbodyComponent, weaponComponent, healthComponent):
    for name, obj in inspect.getmembers(sys.modules[scriptName]):
        if inspect.isclass(obj) and issubclass(obj, bot.Bot):
            global myBot
            myBot = obj(transformComponent, rigidbodyComponent, weaponComponent, healthComponent)
            break

    logger = logging.getLogger('')

    logger.setLevel(logging.NOTSET)

    global stream
    stream = io.StringIO()
    streamHandler = logging.StreamHandler(stream)
    fileHandler = logging.FileHandler('%s.log' % myBot.__class__.__name__)
    formatter = logging.Formatter('[%(levelname)s] %(message)s')
    streamHandler.setFormatter(formatter)
    fileHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)
    logger.addHandler(fileHandler)

def tick(input):
    myBot.tick(input)

def onSeenNewBot(input):
    myBot.onSeenNewBot(input)

def onSeenNewWeapon(input):
    myBot.onSeenNewWeapon(input)

def onSeenNewHealth(input):
    myBot.onSeenNewHealth(input)

def onSeenLostBot(input):
    myBot.onSeenLostBot(input)

def onSeenLostWeapon(input):
    myBot.onSeenLostWeapon(input)

def onSeenLostHealth(input):
    myBot.onSeenLostHealth(input)

def onWeaponPicked(input):
    myBot.onWeaponPicked(input)

def onHealthPicked(input):
    myBot.onHealthPicked(input)

def onBulletHit(input):
    myBot.onBulletHit(input)

def onBulletMiss(input):
    myBot.onBulletMiss(input)

def onHitByBullet(input):
    myBot.onHitByBullet(input)

def onHitWall(input, collisionEvent):
    myBot.onHitWall(input, collisionEvent)

def log():
    content = stream.getvalue()
    for line in content.splitlines(False):
        botbattles.log('\n%s' % line)
    stream.seek(0)
    stream.truncate(0)