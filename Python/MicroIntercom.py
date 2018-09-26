import pythonosc
from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client
import time
from threading import Thread
from multiprocessing import Process
import random

class MicroIntercom(Thread):

    def blocker(self):
        while True:
            print("Oh, sorry, am I in the way?")
            time.sleep(1)

    def __init__(self, inputIp, inputPort, remoteIp, outputPort):
        super(MicroIntercom, self).__init__()

        self.syncedFunctionDict = {}

        self.daemon = True
        self.cancelled = False

        self.dispatcher = dispatcher.Dispatcher()
        self.server = osc_server.ThreadingOSCUDPServer((inputIp, inputPort), self.dispatcher)
        print("Serving on {}".format(self.server.server_address))
        self.client = udp_client.SimpleUDPClient(remoteIp, outputPort)
        self.client.send_message("/tester", ["comeon", 1, 2, 3, 4, 5])
        self.syncedFunctions = {}
        self.syncFunctionCalls = {}

    def run(self):
        """Overloaded Thread.run, runs the update
        method once per every 10 milliseconds."""

        while not self.cancelled:
            self.server.serve_forever()

    def cancel(self):
        """End this timer thread"""
        self.cancelled = True

    def __del__(self):
        print("deleted")

    def processSyncedFunction(self, address, *args):
        if address in self.syncedFunctions:
            #print ("processing synced function call...")
            result = self.syncedFunctions[address](address, *args)
            #print ("result:", result)
            self.client.send_message(address, result)

    def processSyncFunctionCallback(self, address, *args):
        self.syncFunctionCalls[address] = args
        print("sync function callback got back with args", args)

    # API:

    def addAsyncFunction(self, address, callback, replace=False):
        if replace and address in self.dispatcher._map:
            del self.dispatcher._map[address]
        self.dispatcher.map(address, callback)
        print("async function added on ", address)

    def addSyncFunction(self, address, callback, replace=False):
        if replace and address in self.dispatcher._map:
            del self.dispatcher._map[address]
        self.syncedFunctions[address] = callback
        self.dispatcher.map(address, self.processSyncedFunction)

    def callAsyncFunction(self, address, data):
        self.client.send_message(address, data)

    def callSyncFunction(self, address, data):
        global syncFunctionCalls
        self.client.send_message(address, data)
        self.syncFunctionCalls[address] = None
        if address in self.dispatcher._map:
            del self.dispatcher._map[address]
        self.dispatcher.map(address, self.processSyncFunctionCallback)
        c = 0
        while ((self.syncFunctionCalls[address] == None)):
            c += 1
            print("waiting... c = ", c, self.syncFunctionCalls[address])
            time.sleep(0.1)
        result = self.syncFunctionCalls[address]
        del self.syncFunctionCalls[address]
        return result

def asyncFuncTest(address, *args):
    print("async func called ", args)

def syncFuncTest(address, *args):
    print("sync func called ", args)
    return [23 * args[0][0], 2 * args[0][0], 12 * args[0][0]]



# mI = MicroIntercom("localhost", 30000, "localhost", 30001)
# mI.start()
# mI.addAsyncFunction("/tester", asyncFuncTest)
# mI.addSyncFunction("/synctest", syncFuncTest)
#
#
# r = mI.callSyncFunction("/multiply", [random.random(), random.random()])
# r = mI.callAsyncFunction("/addcircle", [random.random() * 1024, random.random() * 768])





# c = 0
# while(True):
#     c += 1


# def blocker():
#     while True:
#         print("Oh, sorry, am I in the way?")
#         time.sleep(1)
#
# t = threading.Thread(name='child procs', target=blocker)
# t.start()
#
# # Prove that we passed through the blocking call



print ("No, that's okay")
