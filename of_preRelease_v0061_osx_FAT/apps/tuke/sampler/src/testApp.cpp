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
	playbackSpeed = 1;
	
	// this is the last time that there was a note
	noteLastTime = -10;
	lastNote = 0;
	
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

	
	vision.setup(&gui);
	
	gui.setWhichColumn(1);
	gui.addSlider("Movement Sensitivity", "volumeSensitivity", 0.1, 0, 1, false);
	gui.addSlider("Movement Threshold", "movementThreshold", 0.1, 0, 1, false);
	gui.addToggle("Loop Sound", "loopSound", loopSound);
	gui.addSlider("Loop Duration", "loopDuration", 1.f, 0, 2, false);
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

int lastMaxLevel = -1;
void testApp::update() {
	
	// the vision code works out how much average change there is in each of
	// either vertical or horizontal strips across the screen.
	// this bit of code finds out if the strip with the most change
	// is a different strip from the last strip, and then triggers that note.
	float max = 0;
	int currMaxLevel = -1;
	for(int i = 0; i < vision.levels.size(); i++) {
		if(max<vision.levels[i]) {
			max = vision.levels[i];
			currMaxLevel = i;
		}
	}
	if(lastMaxLevel!=currMaxLevel) {
		printf("Playing note %d %f\n", currMaxLevel, max);
		float volume = ofMap(max, 0, gui.getValueF("volumeSensitivity"), 0, 1);
		if(volume>1) volume = 1;
		if(volume>gui.getValueF("movementThreshold")) { // some threshold
			playSound(volume, 1.f - (float)currMaxLevel/vision.levels.size());
		} else {
			currMaxLevel = -1;
		}
	}
	
	lastMaxLevel = currMaxLevel;

	
	vision.update();
	gui.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofEnableAlphaBlending();
	vision.draw();
	
	// volume control
	//ofSetColor(0xFF0000);
	//ofRect(0, ofGetHeight()*(1.f-inputLevel), ofGetWidth(), ofGetHeight()*inputLevel);
	

	// fade out a note
	if(ofGetElapsedTimef() - noteLastTime < 1.5f) {
		float alpha = ofMap(ofGetElapsedTimef() - noteLastTime, 0, 1.5, 255, 0);
		ofSetColor(50, 50, 255, alpha);
		if(vision.vertical) {
			float height = (float) ofGetHeight()/vision.levels.size();
			ofRect(0, height*lastNote, ofGetWidth(), height);
		} else {
			float width = (float) ofGetWidth() / vision.levels.size();
			ofRect(lastNote*width, 0, width, ofGetHeight());
		}
		
	}
	
	ofDisableAlphaBlending();
	gui.draw();
	
}






void testApp::audioRequested (float * output, int bufferSize, int nChannels) {
	// if we're recording, we want silence!!
	if(recording) {
		memset(output, 0, bufferSize*nChannels*sizeof(float));
	} else {
		// otherwise, maybe we want playback
		for(int i = 0; i < bufferSize; i++) {
			float s = sample.getSample(playbackSpeed);
			for(int channel = 0; channel < nChannels; channel++) {
				output[i*nChannels + channel] = s;
			}
		}
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
	sample.load(recordBuffer, recordPos);
	sample.normalize();
	printf("Recorded %d samples\n", recordPos);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}

void testApp::playSound(float volume, float pitch) {
	int note = valueToNote(pitch);
	playbackSpeed = noteToSpeed(note);
	sample.trigger(volume);
	noteLastTime = ofGetElapsedTimef();
	lastNote = vision.levels.size() - pitch*vision.levels.size();
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	int note = valueToNote(1.f-((float)y/ofGetHeight()));
	playbackSpeed = noteToSpeed(note);
	sample.trigger(1);
	
	gui.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


float testApp::noteToSpeed(int note) {
	return pow(2, (float)note/12.f);
}

int testApp::valueToNote(float value) {

	int notesInScale = 5;
	int maxOctaves = 2;
	
	// how many octaves we want
	value *= maxOctaves;
	
	// how many notes in the scale
	value *= notesInScale;
	
	// this is the position in the scale
	int noteInScale = floor(value);
	
	// this is the chromatic position
	int noteNum = 0;
	
	// work out the octave offset
	noteNum = floor(noteInScale/notesInScale)*12;
	
	// add the note offset
	switch(noteInScale%notesInScale) {
		case 0: noteNum += 0;  break;
		case 1: noteNum += 3;  break;
		case 2: noteNum += 5;  break;
		case 3: noteNum += 7;  break;
		case 4: noteNum += 10; break;
	}
	return noteNum + gui.getValueF("Pitch");
	/*	

		pentatonic[0] = 0;
		pentatonic[1] = 3;
		pentatonic[2] = 5;
		pentatonic[3] = 7;
		pentatonic[4] = 10;
		
		major[0] = 0;
		major[1] = 2;
		major[2] = 4;
		major[3] = 5;
		major[4] = 7;
		major[5] = 9;
		major[6] = 11;
		
		
		minor[0] = 0;
		minor[1] = 2;
		minor[2] = 3;
		minor[3] = 5;
		minor[4] = 7;
		minor[5] = 8;
		minor[6] = 11;*/
		
}

