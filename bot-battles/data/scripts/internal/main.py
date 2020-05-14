# coding: utf-8

import sys
import inspect
import logging
import io

import bot
import robot

stream = None
myRobot = None

def init(scriptName, transformComponent, rigidbodyComponent):
    for name, obj in inspect.getmembers(sys.modules[scriptName]):
        if inspect.isclass(obj) and issubclass(obj, robot.Robot):
            global myRobot
            myRobot = obj(transformComponent, rigidbodyComponent)
            break

    logger = logging.getLogger('')

    logger.setLevel(logging.NOTSET)

    global stream
    stream = io.StringIO()
    streamHandler = logging.StreamHandler(stream)
    fileHandler = logging.FileHandler('%s.log' % myRobot.__class__.__name__)
    formatter = logging.Formatter('[%(levelname)s] %(message)s')
    streamHandler.setFormatter(formatter)
    fileHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)
    logger.addHandler(fileHandler)

def tick(input):
    myRobot.tick(input)

def onHitWall(input, collisionEvent):
    myRobot.onHitWall(input, collisionEvent)

def log():
    content = stream.getvalue()
    for line in content.splitlines(True):
        bot.log(line)
    stream.seek(0)
    stream.truncate(0)