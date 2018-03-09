#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
#ifdef TARGET_OSX
	//ofSetDataPathRoot("../data/");
#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	cam.initGrabber(640, 480);
	
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
    
	//camTracker.setup();
	srcTracker.setup();
	srcTracker.setIterations(25);
    srcTracker.setClamp(4);
	srcTracker.setAttempts(4);

    
    maxFaces = 2;
    multiTracker.setup(maxFaces, cam.getWidth(), cam.getHeight());
    
    for(int i = 0; i < maxFaces; i++) {
        cloneMulti.setup(cam.getWidth(), cam.getHeight());
 
        maskFbo.allocate(settings);
        srcFbo.allocate(settings);
        ofMesh mesh;
        camMesh.push_back(mesh);
        ofImage imgTmp;
        src.push_back(imgTmp);
        
        ofVec2f pos(-1,-1);
        lastGlassesPos.push_back(pos);
    }
    sunglasses.load("acc/sun2.png");
    sunglassVec.push_back(sunglasses);
    sunglasses.load("acc/sun3.png");
    sunglassVec.push_back(sunglasses);
    cap.load("acc/Cap-Trendy.png");
    capVec.push_back(cap);
    cap.load("acc/hat.png");
    capVec.push_back(cap);
    neck.load("acc/neck.jpg");
    
	faces.allowExt("jpg");
	faces.allowExt("png");
	faces.listDir("faces");
	currentFace = 0;
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
        
        multiTracker.update(toCv(cam));
        
        for (int i = 0; i< multiTracker.trackers_active.size(); i++) {

            if (multiTracker.trackers_active[i]->getFound()) {
                
                ofMesh camMeshTmp = multiTracker.trackers_active[i]->getImageMesh();
                camMesh[i] = camMeshTmp;
                camMesh[i].clearTexCoords();
                camMesh[i].addTexCoords(srcPoints);
                
            }
        }
        maskFbo.begin();
        ofClear(0, 255);
        //ofEnableAlphaBlending();
        for (int i = 0; i< multiTracker.trackers_active.size(); i++) {
            
            if (multiTracker.trackers_active[i]->getFound()) {
        camMesh[i].draw();
            }
        }
        //ofDisableAlphaBlending();
        maskFbo.end();
        
        srcFbo.begin();
        ofClear(0, 255);
        src[0].bind();
        ofEnableAlphaBlending();
        for (int i = 0; i< multiTracker.trackers_active.size(); i++) {
            if (multiTracker.trackers_active[i]->getFound()) {
                camMesh[i].draw();
            }
        }
        ofDisableAlphaBlending();
        src[0].unbind();
        srcFbo.end();
        
        cloneMulti.setStrength(10);
        cloneMulti.update(srcFbo.getTexture(), cam.getTexture(), maskFbo.getTexture());
        
        
	}
}

void ofApp::exit() {
    multiTracker.exit();
}

void ofApp::draw() {
	ofSetColor(255);
 
    if(src[0].getWidth() > 0 && (multiTracker.trackers_active[0]->getFound() || multiTracker.trackers_active[1]->getFound()) ) {
            ofEnableAlphaBlending();
            cloneMulti.draw(0, 0);
            ofDisableAlphaBlending();
    }
    else {
       cam.draw(0, 0);
    }
    
    
    for (int i = 0; i< multiTracker.trackers_active.size(); i++) {
        if (multiTracker.trackers_active[i]->getFound()) {
            
            ofxFaceTracker *camTracker;
            camTracker = multiTracker.trackers_active[i];
            
            ofPolyline polyLE = camTracker->getImageFeature(camTracker->LEFT_EYE);
            ofPolyline polyRE = camTracker->getImageFeature(camTracker->RIGHT_EYE);
            ofPolyline polyJAW = camTracker->getImageFeature(camTracker->JAW);
            
            ofMatrix4x4 rotMat = camTracker->getRotationMatrix();
            ofQuaternion quar = rotMat.getRotate();
            float scale = camTracker->getScale();
            
            
            ofEnableAlphaBlending();
            
            //float ratio = ratio = (polyRE.getCentroid2D().x - polyLE.getCentroid2D().x) /sunglasses.getWidth();
            
            float ratio = 0;
            if(i == 0) {
                ratio = 0.02;
            } else {
                ratio = 0.09;
            }
            
            float wd = sunglassVec[i].getWidth() * ratio * scale;
            float ht = sunglassVec[i].getHeight() * ratio * scale;
            
            float x = (polyRE.getCentroid2D().x + polyLE.getCentroid2D().x)/2.0;
            float y = polyRE.getCentroid2D().y;
            
            if(lastGlassesPos[i].x != -1) {
                x = (x + lastGlassesPos[i].x) / 2.0;
                y = (y + lastGlassesPos[i].y) / 2.0;
            }
            lastGlassesPos[i].x = x;
            lastGlassesPos[i].y = y;
            
            ofPushMatrix();
            ofTranslate(x, y);
            ofRotateX(ofRadToDeg(quar.x()) * 2 );
            ofRotateY(ofRadToDeg(quar.y()) * -2 );
            ofRotateZ(ofRadToDeg(quar.z()) * 2 -5);
            ofSetColor(255, 255, 255, 200);
            
            sunglassVec[i].setAnchorPercent(0.5, 0.4);
            sunglassVec[i].draw(0, 0, wd, ht);
            
            ofPopMatrix();
            
            wd = capVec[i].getWidth() * 0.08 * scale;
            ht = capVec[i].getHeight() * 0.085 * scale;
            
            /*
            x = (polyRE.getCentroid2D().x + polyLE.getCentroid2D().x)/2.0;
            y = polyRE.getCentroid2D().y;
            */
            
            ofPushMatrix();
            ofTranslate(x, y);
            ofRotateX(ofRadToDeg(quar.x()) *-2 +25);
            ofRotateY(ofRadToDeg(quar.y()) *-2 );
            ofRotateZ(ofRadToDeg(quar.z()) * 2 );
            ofSetColor(255, 255, 255, 230);
            
            capVec[i].setAnchorPercent(0.5, 1.1);
            capVec[i].draw(0, 0, wd, ht);
            
            ofPopMatrix();
            
            wd = neck.getWidth() * 0.04 * scale;
            ht = neck.getHeight() * 0.03 * scale;
            
            x = polyJAW.getCentroid2D().x;
            y = polyJAW.getCentroid2D().y;
            
            ofPushMatrix();
            ofTranslate(x, y);
            
            ofSetColor(255, 255, 255, 150);
            
            neck.setAnchorPercent(0.5, -0.85);
            neck.draw(0, 0, wd, ht);
            
            ofPopMatrix();
            
            ofDisableAlphaBlending();
        }
    }
}

void ofApp::loadFace(string face){
    for(int i=0;i<maxFaces;i++) {
        ofImage srcTmp;
        srcTmp.load(face);
        src[i] = srcTmp;
    }
    if(src[0].getWidth() > 0) {
        srcTracker.update(toCv(src[0]));
        srcPoints = srcTracker.getImagePoints();
    }
    
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
	loadFace(dragInfo.files[0]);
}

void ofApp::keyPressed(int key){
    
    
	switch(key){
	case OF_KEY_UP:
		currentFace++;
		break;
	case OF_KEY_DOWN:
		currentFace--;
		break;
	}
	currentFace = ofClamp(currentFace,0,faces.size()-1);
	if(faces.size()!=0){
        //cout<<"currentFace "<< currentFace << " size " <<faces.size()<< faces.getPath(currentFace)<<endl;
		loadFace(faces.getPath(currentFace));
	}
}
