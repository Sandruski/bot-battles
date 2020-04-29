# coding: utf-8

import sys
import inspect
import logging
import io

import bot

import robot

stream = None
myRobot = None

def init(scriptName, transformComponent):
    for name, obj in inspect.getmembers(sys.modules[scriptName]):
        if inspect.isclass(obj) and issubclass(obj, robot.Robot):
            global myRobot
            myRobot = obj(transformComponent)
            break

    logger = logging.getLogger('')

    logger.setLevel(logging.NOTSET)

    global stream
    stream = io.StringIO()
    handler = logging.StreamHandler(stream)
    formatter = logging.Formatter('[%(levelname)s] %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

def update(input):
    myRobot.tick(input)

    content = stream.getvalue()
    for line in content.splitlines(True):
        bot.log(line)
    stream.seek(0)
    stream.truncate(0)