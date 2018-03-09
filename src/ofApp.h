#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxFaceTrackerMulti.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void dragEvent(ofDragInfo dragInfo);
	void loadFace(string face);
    void exit();
	void keyPressed(int key);

	ofxFaceTrackerThreaded camTracker;
	ofVideoGrabber cam;
	
	ofxFaceTracker srcTracker;
	vector<ofImage> src;
	vector<ofVec2f> srcPoints;
	
	bool cloneReady;
	Clone cloneMulti;
	ofFbo srcFbo, maskFbo;
    vector<ofMesh> camMesh;
    
    ofImage sunglasses;
    ofImage cap;
     
    ofImage neck;
    
    
    vector<ofImage> sunglassVec;
    vector<ofImage> capVec;
    vector<ofImage> neckVec;
    
    
	ofDirectory faces;
	int currentFace;
    
    ofxFaceTrackerMulti multiTracker;
    int maxFaces;
    
    vector<ofVec2f> lastGlassesPos;

    
};
