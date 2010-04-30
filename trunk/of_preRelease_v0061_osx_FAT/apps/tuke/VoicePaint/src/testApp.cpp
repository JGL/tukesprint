#include "testApp.h"
#include "stdio.h"


#define BUFFER_SIZE 2048
#define NUM_BARS 128.f
float fftSmoothing = 0.9;
float threshold = 0.04;
bool zoom = false;
float volume = 0;
float pitch = 0;
//--------------------------------------------------------------
void testApp::setup(){	 
	
	ofBackground(0,0,0);	
	ofSoundStreamSetup(0,1,this, 44100, BUFFER_SIZE, 1);	
	fft = ofxFft::create(BUFFER_SIZE, OF_FFT_WINDOW_BARTLETT);
	fftOutput = new float[fft->getBinSize()];
	fftOutputSmoothed = new float[fft->getBinSize()];
	memset(fftOutputSmoothed, 0, fft->getBinSize()*sizeof(float));
	
	inputFilter.q = 1;
	inputFilter.type = HI;
	inputFilter.calc_filter_coeffs(150);
}

void testApp::update() {
	if(volume>threshold) {
		// find highest peak
		float max = 0;
		int maxI = -1;
		for(int i = 0; i < NUM_BARS; i++) {
			if(max<fftOutputSmoothed[i]) {
				max = fftOutputSmoothed[i];
				maxI = i;
			}
		}
		if(maxI>-1) {
			pitch = maxI;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(0xFFFFFF);
	
	float colWidth = (float)ofGetWidth()/NUM_BARS;
	for(int i = 0; i < NUM_BARS; i++) {
		ofRect(colWidth*i, 0, colWidth, ofGetHeight()*fftOutputSmoothed[i]*5.f);
	}
	ofSetColor(0xFF0000);
	ofRect(0, ofGetHeight()-20, ofGetWidth()*volume, 20);
	ofSetColor(0xFFFF00);
	ofCircle(10+pitch*ofGetWidth()/NUM_BARS, ofGetHeight()/2, 20);
}





//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	
	
	inputFilter.filter(input, bufferSize);
	inputFilter.filter(input, bufferSize);
	
	fft->setSignal(input);
	fftOutput = fft->getAmplitude();
	int binSize = fft->getBinSize();

	for(int i = 0; i < binSize; i++) {
		if(fftOutput[i]<threshold) {
			fftOutput[i] = 0;
		}
		fftOutputSmoothed[i] = fftOutputSmoothed[i]*(fftSmoothing) + (1.f-fftSmoothing)*fftOutput[i];
	}

	for(int i = 0; i < bufferSize; i++) {
		if(input[i]>volume) {
			volume = input[i];
		} else {
			volume *= 0.9999;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
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
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}



