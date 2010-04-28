
#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp() {	
}

//--------------------------------------------------------------
void testApp::setup() {
	
	
	
	ofSetVerticalSync(true);
	ofBackground(20, 20, 20);
	ofSetLogLevel(OF_LOG_NOTICE);
	//ofToggleFullscreen();
	bDrawLines  = false;
	bMouseForce = false;
	
	box2d.init();
	box2d.setGravity(0, 20);
	
	//this call is creating the floor at the bottom of the window; not the screen!?
	box2d.createFloor(1440,850);
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	ofSoundStreamSetup(0,2,this, 44100, 256, 1);	
	left = new float[256];
	right = new float[256];
	
	
	bufferCounter = 0;
	drawCounter = 0;
	
	//lineStrip.setWorld(box2d.getWorld());
	//lineStrip.clear();
	
	//lineStrip.addPoint(0,50);
	//lineStrip.addPoint(1440,50);
	//lineStrip.createShape();
	//setting up the upDirection and upForce
	upDirection.x = 20;
	upDirection.y = 20;
	upForce.x = 20;
	upForce.y = 10;
	threshold=0.01;
	
}

//--------------------------------------------------------------
void testApp::update() {
	
	
	
	box2d.update();
	
	generateParticles();
	//erase old particles from the vector
	//customParticles[0].
	
	
	if(customParticles.size()>100){
		delete customParticles[0];
		customParticles.erase(customParticles.begin() );
	}
	
}



//--------------------------------------------------------------
void testApp::draw() {
	
	for(int i=0; i<customParticles.size(); i++) {
		customParticles[i]->draw();
	}
	
	px = mouseX;
	py = mouseY;
	
	
	string info = "";
	info += "Press [s] to draw a line strip ["+ofToString(bDrawLines)+"]\n"; 
	info += "Press [f] to toggle Mouse Force ["+ofToString(bMouseForce)+"]\n"; 
	info += "Press [c] for circles\n";
	info += "Press [b] for blocks\n";
	info += "Press [z] for custom particle\n";
	info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
	info += "Total Joints: "+ofToString(box2d.getJointCount())+"\n\n";
	info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
	ofSetColor(255, 255, 255);
	//ofDrawBitmapString(info, 30, 30);
	
	///audio stuff
	ofNoFill();
	ofSetColor(255,0,0);
	
	for (int i = 0; i < 256; i++){
		myMean+=(left[i]+right[i]);
		//ofCircle(ofGetScreenWidth()/2,ofGetScreenHeight()/2, (left[i]+right[i])*100);
	}
	cout << threshold<<" threshold\n";
	
	myMean/=256;
	if(myMean>threshold){
		for (int i=0; i<customParticles.size(); i++){
			
			customParticles[i]->addAttractionPoint((ofGetWidth()/2,0,0), 0, 1.0f, 1);
			customParticles[i]->addImpulseForce(upDirection.y, upForce.y);
			
		}
	}
	
}

//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	
	
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		left[i] = input[i*2];
		right[i] = input[i*2+1];
	}
	bufferCounter++;
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	//setting the threshold...
	if(key == 'k') {
		threshold+=0.01;
	}
	if(key == 'm') {
		threshold-=0.01;
	}
	if(key == 'f') {
		ofToggleFullscreen();
	}
	if(key == 'z') {
		/*
		 float r = ofRandom(3, 10);		// a random radius 4px - 20px
		 CustomParticle *p = new CustomParticle();
		 //p.setPhysics(0.4, 0.53, 0.31);
		 p->setPhysics(0.9, 0.53, 0.0);
		 p.setup(box2d.getWorld(), mouseX, mouseY, r);
		 p.color.r = ofRandom(20, 100);
		 p.color.g = 0;
		 p.color.b = ofRandom(150, 255);
		 customParticles.push_back(p);*/
	}	
	if (key=='A' || key=='a'){
		for (int i=0; i<customParticles.size(); i++){
			customParticles[i]->addAttractionPoint((ofGetWidth()/2,0,0), 0, 1.0f, 1);
			customParticles[i]->addImpulseForce(upDirection.y, upForce.y);
			
			//upDirection.y=abs(myMean*200);
			//upForce.y=abs(myMean*200);
			
			cout << upDirection.y<<" mymeam\n";
			/*for (int i=0; i<customParticles.size(); i++){
			 customParticles[i].addAttractionPoint((ofGetWidth()/2,0,0), 0, 1.0f, 1);
			 customParticles[i].addImpulseForce(upDirection.y, upForce.y);
			 }*/
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(bDrawLines) lineStrip.createShape();
	
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
	
}

void testApp::generateParticles(){
	float r = ofRandom(3, 10);		// a random radius 4px - 20px
	CustomParticle *p = new CustomParticle();
	//p.setPhysics(0.4, 0.53, 0.31);
	p->setPhysics(0.9, 0.53, 0.0);
	//p.setup(box2d.getWorld(), ofRandom(0,ofGetWidth()),2*(ofGetHeight()/3), r);
	p->setup(box2d.getWorld(), ofRandom(0,1440),820, r);
	p->color.r = ofRandom(20, 100);
	p->color.g = 0;
	p->color.b = ofRandom(150, 255);
	customParticles.push_back(p);
	
}


