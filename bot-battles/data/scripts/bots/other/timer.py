import time

class Timer:
    def __init__(self):
        self._startTime = 0.0

    def restart(self):
        self.stop()
        self.start()

    def start(self):
        self._startTime = time.perf_counter()

    def read(self):
        return time.perf_counter() - self._startTime

    def stop(self):
        self._startTime = 0.0