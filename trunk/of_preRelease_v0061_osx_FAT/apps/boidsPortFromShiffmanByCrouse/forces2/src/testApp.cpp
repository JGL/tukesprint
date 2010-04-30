#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(36);
	doCapture = false;
	framenum = 1;
	
	
	gravity.y = 0.08;
	
	liq_start    = 120; // Liquid location
	liq_height   = 90; // Liquid height
}

//--------------------------------------------------------------
void testApp::update(){
	t.applyForce(gravity);
	t.applyForce(wind);
	
	if ((t.loc.y > liq_start) && (t.loc.y < liq_start + liq_height)) {
		// Drag is calculated as force vector in the negative direction of velocity
		float c = -0.03;                            // Drag coefficient
													// Velocity of our thing
		ofPoint force = t.vel * c;					// Following the formula above
		t.applyForce(force);                        // Adding the force to our object, which will ultimately affect its acceleration
	}
	t.update();
}

//--------------------------------------------------------------
void testApp::draw(){

	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 50);
	ofFill();
	ofRect(0, liq_start, ofGetWidth(), liq_height);
	ofDisableAlphaBlending();

	t.draw();
	
	Thing::drawVector(t.loc, wind, 100);
	Thing::drawVector(t.loc, gravity, 100);
	
	capture();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key=='c') doCapture = !doCapture;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	wind.x = ofMap(x, 0, ofGetWidth(), -0.09, 0.09);
	wind.y = .01;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::capture() {
	if(doCapture && ofGetFrameNum() % 4 == 0)
	{
		char filename[255];
		sprintf(filename, "frame%05d.png", framenum++);
		ofSaveScreen(filename);
	}
}