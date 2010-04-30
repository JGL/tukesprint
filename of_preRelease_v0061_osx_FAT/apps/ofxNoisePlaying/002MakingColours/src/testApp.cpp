#include "testApp.h"


	//--------------------------------------------------------------
void testApp::setup(){	 
	ofBackground(0,0,0);
	
	noise = new ofxPerlin();
	
	w = 64;
	h = 64;
	
	texColor.allocate(w,h,GL_RGB);
	
	colorPixels 		= new unsigned char [w*h*3];
}

	//--------------------------------------------------------------
void testApp::update(){
	
		// color pixels, use w and h to control red and green
	for (int i = 0; i < w; i++){
		for (int j = 0; j < h; j++){
			colorPixels[(j*w+i)*3 + 0] = noise->noiseuf((j*w+i)*3 + 0,(float)ofGetFrameNum())*255;	// r
			colorPixels[(j*w+i)*3 + 1] = noise->noiseuf((j*w+i)*3 + 1,(float)ofGetFrameNum())*255;
			colorPixels[(j*w+i)*3 + 2] = noise->noiseuf((j*w+i)*3 + 2,(float)ofGetFrameNum())*255;
		}
	}
	
	texColor.loadData(colorPixels, w,h, GL_RGB);
}

	//--------------------------------------------------------------
void testApp::draw(){
//	ofEnableAlphaBlending();
//	ofSetColor(255, 255, 255, 127);
//	ofLine(0, ofGetHeight()/2, ofGetWidth(), ofGetHeight()/2);
//	
//	
//		//3d perlin noise
//	float div = 100; 
//	ofBeginShape();
//	ofVertex(0, ofGetHeight());
//	for(int i=0; i<ofGetScreenWidth(); i+=2){
//		float posY = noise->noise((float)i/div, (float)mouseX/div, (float)ofGetFrameNum()/div);
//		posY *= 200;
//		posY += ofGetHeight()/2;
//		ofVertex(i, posY);
//	}
//	ofVertex(ofGetWidth(), ofGetHeight());
//	ofEndShape();
//	
//	ofDisableAlphaBlending();
	
		//now lets get a texture going
	
	texColor.draw(20, 20);
}

	//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
}

	//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
	
}

	//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

	//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

	//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

	//--------------------------------------------------------------
void testApp::mouseReleased(){
	
}
