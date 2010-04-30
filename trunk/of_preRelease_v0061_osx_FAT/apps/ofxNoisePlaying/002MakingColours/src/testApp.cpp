#include "testApp.h"


	//--------------------------------------------------------------
void testApp::setup(){	 
	ofBackground(0,0,0);
	
	noise = new ofxPerlin();
	
	w = 64;
	h = 64;
	
	texColor.allocate(w,h,GL_RGB);
	
	colorPixels 		= new unsigned char [w*h*3];
	
		//noise->noiseDetail(8,0.5);
}

float cloudfilter(float val) {
	return (1-pow(0.9f,MAX(val-0.55f,0)*255));
}

	//--------------------------------------------------------------
void testApp::update(){
	
	float red,green,blue;
	
	red = 100;
	green = 200;
	blue = 300;
	
	float ns = 0.01f;
	float z=ofGetElapsedTimef()/10.f;
		// color pixels, use w and h to control red and green
//	for (int i = 0; i < w; i++){
//		for (int j = 0; j < h; j++){
//			colorPixels[(j*w+i)*3 + 0] = cloudfilter(noise->noiseuf(i*ns+red,j*ns,z))*255;	// r
//			colorPixels[(j*w+i)*3 + 1] = cloudfilter(noise->noiseuf(i*ns+green,j*ns,z))*255;
//			colorPixels[(j*w+i)*3 + 2] = cloudfilter(noise->noiseuf(i*ns+blue,j*ns,z))*255;
//		}
//	}

	for (int i = 0; i < w; i++){
		for (int j = 0; j < h; j++){
			colorPixels[(j*w+i)*3 + 0] = noise->noiseuf(i*ns+red,j*ns,z)*255;	// r
			colorPixels[(j*w+i)*3 + 1] = noise->noiseuf(i*ns+green,j*ns,z)*255;
			colorPixels[(j*w+i)*3 + 2] = noise->noiseuf(i*ns+blue,j*ns,z)*255;
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
	
	texColor.draw(0, 0, ofGetWidth(), ofGetHeight());
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
