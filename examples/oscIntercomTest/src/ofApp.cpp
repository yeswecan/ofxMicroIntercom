#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    ofxOscMessage m;
//    m.setAddress("/tester");
//    m.addIntArg(32);
//    m.addFloatArg(16.5);
//    m.addFloatArg(28);
//    s.sendMessage(m);
//
//

    intercom.setup(30001, 30000);
    
    intercom.callAsync("/yo", std::vector<float>());
    
    intercom.addAsyncFunction("/addcircle", [&](IMessage m) {
        circles.push_back(ofPoint(m.asFloatVector()[0], m.asFloatVector()[1]));
    });

    intercom.addSyncFunction("/multiply", [&](IMessage m)->IMessage {
        ofLog() << "sync function called";
        return {m.asFloatVector()[0] * m.asFloatVector()[1]};
    });
}

//--------------------------------------------------------------
void ofApp::update(){
	ofVec2f mouse(1.0f * mouseX / ofGetWidth() - 0.5, 1.0f * mouseY / ofGetHeight() - 0.5);
	auto r = intercom.callSync("/mouse",{ mouse.x , mouse.y });
	 
    intercom.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    for (auto i: circles) {
        ofSetColor(255);
        ofDrawCircle(i, 35);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    std::vector<float> f;
    f.push_back(0.25);
    
    auto i = intercom.callSync("/synctest", {ofRandomuf()}).asFloatVector();
    if(i.size() > 0) ofLog() << "got response " << i[0] << " " << i[1] << " " << i[2];
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
