#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Sample.h"
#include "Vision.h"

#include "ofxControlPanel.h"

#define MAX_RECORD_SECONDS 10
#define SAMPLERATE 44100
class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void exit();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void audioRequested (float * output, int bufferSize, int nChannels); 		
	void audioReceived 	(float * input, int bufferSize, int nChannels); 
	
	Sample sample;
	float *recordBuffer;
	int recordBufferSize;
	int recordPos;
	
	bool recording;
	
	float playbackSpeed;
	
	// how loud the microphone is.
	float inputLevel;
	
	// give this function a normalized
	// value, and it gives back a note number
	int valueToNote(float y);
	
	// converts a note number to sample playback speed
	float noteToSpeed(int note);
	
	// both volume and pitch go from 0 to 1
	void playSound(float volume, float pitch);

	
	// takes care of all the vision
	Vision vision;
	
	ofxControlPanel gui;
	float noteLastTime;
	int lastNote;
	bool loopSound;
	float loopDuration;
};

#endif	

