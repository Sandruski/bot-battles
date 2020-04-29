import sys
import inspect
import logging
import io
import bot

myStream = None
myBot = None

class Bot:
    def __init__(self, transformComponent):
        self.transformComponent = transformComponent

    def tick(self, input):
        position = np.array(self.transformComponent.position, copy=False)
        position = 10

def init(scriptName, transformComponent):
    for name, obj in inspect.getmembers(sys.modules[scriptName]):
        if inspect.isclass(obj) and issubclass(obj, Bot):
            global myBot
            myBot = obj(transformComponent)
            break

    logger = logging.getLogger('')

    logger.setLevel(logging.NOTSET)

    global myStream
    myStream = io.StringIO()
    handler = logging.StreamHandler(myStream)
    formatter = logging.Formatter('[%(levelname)s] %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

def update(input):
    myBot.tick(input)
    content = myStream.getvalue()
    for line in content.splitlines(True):
        bot.log(line)
    myStream.seek(0)
    myStream.truncate(0)