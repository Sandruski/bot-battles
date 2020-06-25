import time

class Timer:
    def __init__(self):
        self._startTime = None

    def restart(self):
        self.stop()
        self.start()

    def start(self):
        if self._startTime != None:
            return

        self._startTime = time.perf_counter()

    def read(self):
        if self._startTime == None:
            return

        return time.perf_counter() - self._startTime

    def stop(self):
        if self._startTime == None:
            return

        self._startTime = None