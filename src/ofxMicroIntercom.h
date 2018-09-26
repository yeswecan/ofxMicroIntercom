#pragma once

#include "ofxOsc.h"

class IMessage: public ofxOscMessage {
public:
    
    operator std::vector<float> () {
        // TODO: return list of floats
    }

	IMessage() {
	}

    IMessage(ofxOscMessage m) {
        ((ofxOscMessage*)this)->operator=(m);
    }
    
    IMessage(const std::vector<float> f){
        for (auto i: f) addFloatArg(i);
    }
    
    IMessage(std::initializer_list<int> a_args)
    {
        for (auto i: a_args) addIntArg(i);
    }

    IMessage(std::initializer_list<double> a_args)
    {
        for (auto i: a_args) addDoubleArg(i);
    }

    IMessage(std::initializer_list<float> a_args)
    {
        for (auto i: a_args) addFloatArg(i);
    }

    IMessage(std::initializer_list<std::string> a_args)
    {
        for (auto i: a_args) addStringArg(i);
    }

    std::vector<int> asIntVector() {
        std::vector<int> f;
        for (int i = 0; i < getNumArgs(); i++) {
            f.push_back(getArgAsInt(i));
        }
        return f;
    }

	std::vector<float> asFloatVector() {
		std::vector<float> f;
		for (int i = 0; i < getNumArgs(); i++) {
			f.push_back(getArgAsFloat(i));
		}
		return f;
	}
};

class ofxMicroIntercom {
    ofxOscReceiver r;
    ofxOscSender s;
    
public:
    
    void setup(int inputPort, int outputPort) {
        r.setup(inputPort);
        
        s.setup("127.0.0.1", outputPort);
        
    }
    
    void callAsync(std::string address, IMessage m) {
        sendMessageThroughMutex(m);
    }
    
    std::map<std::string, IMessage> receivedMessages;
    
    std::mutex senderMutex;
    
    void sendMessageThroughMutex(IMessage message) {
        senderMutex.lock();
                s.sendMessage(message);
        senderMutex.unlock();
    }
    
    std::map<std::string, std::function<void(IMessage)>> asyncFunctions;
    std::map<std::string, std::function<IMessage(IMessage)>> syncFunctions;

    void addAsyncFunction(std::string address,
                          std::function<void(IMessage)> callback) {
        asyncFunctions[address] = callback;
    }

    void addSyncFunction(std::string address,
                          std::function<IMessage(IMessage)> callback) {
        syncFunctions[address] = callback;
    }

    // TODO: Message class that encapsulates stuff
    // More versions of this for different inputs/outputs
    IMessage callSync(std::string address, IMessage m, double timeout = 5.0) {

//        callSyncLock = true;
		float starttime = ofGetElapsedTimef();

        m.setAddress(address);
        
        sendMessageThroughMutex(m);
        
//        callSyncLock = false;
        
        // Now wait for the response
        bool found = false;
        while (!found && ofGetElapsedTimef() - starttime < timeout) {
            
            update();
            
            for (auto i: receivedMessages) {
                if (i.first == address) {
                    ofLog() << "found!";
                    receivedMessages.erase(i.first);
                    return i.second;
                }
            }
        }
        
		ofLog() << "error: timeout!";

        return IMessage();
    }
    
    void update() {

        
        while (r.hasWaitingMessages()) {
            
            ofxOscMessage msg;
            r.getNextMessage(msg);

            // Checking whether we have endpoints for this messages...
            
            for (auto i: syncFunctions) {
                if (i.first == msg.getAddress()) {
                    IMessage m = msg;
                    IMessage result = i.second(m);
                    result.setAddress(i.first);
                    sendMessageThroughMutex(result);
                }
            }
            
            for (auto i: asyncFunctions) {
                if (i.first == msg.getAddress()) {
                    IMessage m = msg;
                    i.second(m);
                }
            }
            
            // If we don't, put it to receivedMessages
            
            IMessage m = msg;
            receivedMessages.insert(std::make_pair(msg.getAddress(), m));
            
            
        }
    }
};
