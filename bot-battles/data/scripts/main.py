import sys, inspect

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
            return

def update(input):
    myBot.tick(input)
            