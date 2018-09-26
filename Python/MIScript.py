from MicroIntercom import MicroIntercom
import time
import random

mI = MicroIntercom("localhost", 30000, "localhost", 30001)

def looper():
    for i in range(0, 190):
        mI.callAsyncFunction('/noteon', [200 + random.random() * 1000, 0.5])
        time.sleep(random.random() * 0.0001)
        mI.callAsyncFunction('/noteoff', [50])
        time.sleep(random.random() * 0.001 + 0.003)
    return


looper()

mI.callAsyncFunction("/addeffect", ["FreeVerb"])

mI.callAsyncFunction("/setparameters", [0, "FreeVerb", 0.00015, -0.0015, 0.0])
looper()

idx = 0
while (True):
    idx += 1

