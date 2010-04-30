#include "testApp.h"
#include "stdio.h"
#include "ofxDirList.h"


pthread_mutex_t         controlMutex = PTHREAD_MUTEX_INITIALIZER;


#define PENTATONIC "pentatonic"
#define MAJOR "major"
#define MINOR "minor"
#define CHROMATIC "chromatic"

ofImage Particle::particle;
ofxControlPanel *Particle::gui;
ofColor *Particle::color;
//--------------------------------------------------------------
void testApp::setup(){	 
	
	ofBackground(0,0,0);	
	Particle::gui = &gui;
	Particle::color = &particleColorPicker.col;
	
	recordBufferSize = SAMPLERATE*MAX_RECORD_SECONDS;
	recordBuffer = new float[recordBufferSize];
	recordPos = 0;
	recording = false;
	inputLevel = 0;
	playbackSpeed = 1;
	lastSound = -1;
	
	
	ofxDirList DIR;
	
	int numFiles = DIR.listDir("./../sounds");
	for(int i = 0; i < numFiles; i++) {
		sounds.push_back(DIR.getName(i));
	}
	
	
	// this is the last time that there was a note
	noteLastTime = -10;
	lastNote = 0;
	
	
	scales.push_back(PENTATONIC);
	scales.push_back(MAJOR);
	scales.push_back(MINOR);
	scales.push_back(CHROMATIC);

	
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
	gui.setup("Sampler", 0, 0, 800, 700);
	gui.addPanel("sampler", 4, false);
		
	//--------- PANEL 1

	
	vision.setup(&gui);
	
	gui.setWhichColumn(1);
	gui.addSlider("Pitch", "Pitch", 0, -20, 20, true);
	gui.addTextDropDown("Scale", "scale", 0, scales);
	gui.addSlider("Movement Sensitivity", "volumeSensitivity", 0.1, 0, 1, false);
	gui.addSlider("Movement Threshold", "movementThreshold", 0.1, 0, 1, false);
	gui.setWhichColumn(2);
	gui.addSlider("Star Points", "starPoints", 10, 3, 100, true);
	gui.addCustomRect("Star colour", &particleColorPicker, 200, 200);
	gui.addSlider("Star Size", "starSize", 100, 10, 300, false);
	gui.addSlider("Star Speed", "starSpeed", 50, 10, 120, false);
	gui.addSlider("Star Lifetime", "starMaxAge", 100, 10, 200, true);
	gui.addTextDropDown("Sound", "sound", 0, sounds);
	
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
		//printf("Playing note %d %f\n", currMaxLevel, max);
		float volume = ofMap(max, 0, gui.getValueF("volumeSensitivity"), 0, 1);
		if(volume>1) volume = 1;
		if(volume>gui.getValueF("movementThreshold")) { // some threshold
			lastMaxLevel = currMaxLevel;
			playSound(volume, 1.f - (float)currMaxLevel/vision.levels.size());
		} else {
			currMaxLevel = -1;
		}
	}
	
	lastMaxLevel = currMaxLevel;

	
	int soundIndex = gui.getValueI("sound");
	if(soundIndex!=lastSound) {
		printf("loading %s\n", sounds[soundIndex].c_str());
		string file = "./../sounds/" + sounds[soundIndex];
		pthread_mutex_lock(&controlMutex);
		sample.loadFromFile(file);
		pthread_mutex_unlock(&controlMutex);
	}
	lastSound = soundIndex;
	
	vision.update();
	gui.update();
	for(int i = 0; i < particles.size(); i++) {
		particles[i].update();
		if(!particles[i].alive) {
			particles.erase(particles.begin()+i);
			i--;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofFill();
	ofEnableAlphaBlending();
	vision.draw();

	// fade out a note
	if(ofGetElapsedTimef() - noteLastTime < 1.5f) {
		float alpha = 0.5*ofMap(ofGetElapsedTimef() - noteLastTime, 0, 1.5, 255, 0);
		
		//if(vision.vertical) {
		float height = (float) ofGetHeight()/vision.levels.size();
		//ofRect(0, height*lastNote, ofGetWidth(), height);
		
		glBegin(GL_QUAD_STRIP);
		ofSetColor(vision.noteColorPicker.col.r, vision.noteColorPicker.col.g, vision.noteColorPicker.col.b, alpha/2);
		glVertex2f(0, height*lastNote);
		glVertex2f(ofGetWidth(), height*lastNote);
		
		ofSetColor(vision.noteColorPicker.col.r, vision.noteColorPicker.col.g, vision.noteColorPicker.col.b, alpha);
		glVertex2f(0, height*lastNote+height/2);
		glVertex2f(ofGetWidth(), height*lastNote + height/2);
		
		ofSetColor(vision.noteColorPicker.col.r, vision.noteColorPicker.col.g, vision.noteColorPicker.col.b, alpha/2);
		glVertex2f(0, height*lastNote+height);
		glVertex2f(ofGetWidth(), height*lastNote + height);
		
		glEnd();
		
	}
	

	

	ofSetColor(0xFFFFFF);

	for(int i = 0; i < particles.size(); i++) {
		particles[i].draw();
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


	pthread_mutex_lock(&controlMutex);
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
	pthread_mutex_unlock(&controlMutex);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if(key==' ') gui.toggleView();
	else if(key=='f') ofToggleFullscreen();
	
	else if(!recording) { // this stops that weird key repeating thing
		recordPos = 0;
		recording = true;
		//printf("Key pressed\n");
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
	if(recording) {
		recording = false;
		sample.load(recordBuffer, recordPos);
		sample.normalize(0.5);
	}
	//printf("Recorded %d samples\n", recordPos);
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
	
	// do some stuff here
	ofPoint pos, force;
	if(vision.getBiggestFlowPoint(&pos, &force)) {
		// correct the y pos
		float y = (float)((float)lastMaxLevel+0.5)/vision.levels.size();
		pos.y = y;
		spawnParticle(pos, volume);
	}
}

void testApp::spawnParticle(ofPoint pos, float volume) {
	int star = gui.getValueI("starPoints");
	for(int i = 0; i < star; i++) {
		float angle = (float)i/star;
		angle *= 2*PI;
		particles.push_back(Particle(pos.x*ofGetWidth(), pos.y*ofGetHeight(), volume, angle));
	}
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

	int scale = gui.getValueI("scale");
	int notesInScale = 5;
	if(scales[scale]==PENTATONIC) {
		notesInScale = 5;
	} else if(scales[scale]==MAJOR) {
		notesInScale = 7;
	} else if(scales[scale]==MINOR) {
		notesInScale = 7;
	} else if(scales[scale]==CHROMATIC) {
		notesInScale = 12;
	}
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
	
	
	
	if(scales[scale]==PENTATONIC) {
		switch(noteInScale%notesInScale) {
			case 0: noteNum += 0;  break;
			case 1: noteNum += 3;  break;
			case 2: noteNum += 5;  break;
			case 3: noteNum += 7;  break;
			case 4: noteNum += 10; break;
		}
	} else if(scales[scale]==MAJOR) {
		switch(noteInScale%notesInScale) {
			case 0: noteNum += 0;  break;
			case 1: noteNum += 2;  break;
			case 2: noteNum += 4;  break;
			case 3: noteNum += 5;  break;
			case 4: noteNum += 7; break;
			case 5: noteNum += 9;  break;
			case 6: noteNum += 11;  break;

		}
	} else if(scales[scale]==MINOR) {
		switch(noteInScale%notesInScale) {
			case 0: noteNum += 0;  break;
			case 1: noteNum += 2;  break;
			case 2: noteNum += 3;  break;
			case 3: noteNum += 5;  break;
			case 4: noteNum += 7;  break;
			case 5: noteNum += 8;  break;
			case 6: noteNum += 11; break;
		}
	} else if(scales[scale]==CHROMATIC) {
		noteNum += noteInScale%notesInScale;
	}
	
	
	return noteNum + gui.getValueF("Pitch");
			
}

