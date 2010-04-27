#include "testApp.h"
#include "stdio.h"




//--------------------------------------------------------------
void testApp::setup(){	 
	
	ofBackground(0,0,0);	
	
	
	recordBufferSize = SAMPLERATE*MAX_RECORD_SECONDS;
	recordBuffer = new float[recordBufferSize];
	recordPos = 0;
	recording = false;
	inputLevel = 0;
	
	memset(recordBuffer, 0, recordBufferSize*sizeof(float));
	// 1 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 1 num buffers (latency)
	ofSoundStreamSetup(2,1,this, SAMPLERATE, 256, 1);	
	ofSetFrameRate(60.f);
	
	
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 60, 200));
	ofxControlPanel::setTextColor(simpleColor(240, 50, 50, 255));
	
	gui.loadFont("isocpeur.ttf", 8);		
	gui.setup("Sampler", 0, 0, ofGetWidth(), 700);
	gui.addPanel("background subtraction example", 4, false);
		
	//--------- PANEL 1

	
	
	gui.setWhichColumn(1);
	gui.addSlider("Movement Sensitivity", "volumeSensitivity", 0.1, 0, 1, false);
	gui.addSlider("Movement Threshold", "movementThreshold", 0.1, 0, 1, false);
	//gui.addToggle("Loop Sound", "loopSound", loopSound);
	//gui.addSlider("Loop Duration", "loopDuration", 1.f, 0, 2, false);
	gui.addSlider("Pitch", "pitch", 0, -20, 20, true);
	//load from xml!
	gui.loadSettings("controlPanelSettings.xml");
	
	//if you want to use events call this after you have added all your gui elements
	gui.setupEvents();
	gui.enableEvents();	
	gui.hide();

}
void testApp::exit() {
	gui.saveSettings("controlPanelSettings.xml");
}

void testApp::update() {
	
	gui.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofEnableAlphaBlending();

	
	// volume control
	//ofSetColor(0xFF0000);
	//ofRect(0, ofGetHeight()*(1.f-inputLevel), ofGetWidth(), ofGetHeight()*inputLevel);
	

	ofDisableAlphaBlending();
	gui.draw();
	
}






void testApp::audioRequested (float * output, int bufferSize, int nChannels) {
	// if we're recording, we want silence!!
	if(recording) {
		memset(output, 0, bufferSize*nChannels*sizeof(float));
	} else {
		// otherwise, maybe we want playback
		/*for(int i = 0; i < bufferSize; i++) {
			float s = sample.getSample(playbackSpeed);
			for(int channel = 0; channel < nChannels; channel++) {
				output[i*nChannels + channel] = s;
			}
		}*/
	}
}
//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	


	for(int i = 0; i < bufferSize; i++) {
		
		float inp = input[i*nChannels];
		
		// do the recording
		if(recording && recordPos<recordBufferSize) {
			recordBuffer[recordPos++] = inp;
		}
		// do a level meter
		if(inputLevel<inp) {
			inputLevel = inp;
		} else {
			inputLevel *= 0.99995;
		}
	}

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if(key=='h') gui.toggleView();
	if(key=='f') ofToggleFullscreen();
	
	if(!recording) { // this stops that weird key repeating thing
		recordPos = 0;
		recording = true;
		printf("Key pressed\n");
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
	recording = false;
	//sample.load(recordBuffer, recordPos);
	//sample.normalize();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}


//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	gui.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
