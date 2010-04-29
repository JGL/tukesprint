#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "VideoSample.h"
#include "ofxControlPanel.h"

#define MAX_RECORD_SECONDS 10

#define SAMPLERATE		44100


#define MAX_NUM_FRAMES (MAX_RECORD_SECONDS*25)
#define MAX_VIDEOS 12
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
	
	
	void startRecording();
	void stopRecording();

	// audio recording stuff
	float *recordBuffer;
	int recordBufferSize;
	int recordPos;
	
	// video recording stuff
	ofVideoGrabber cam;
	vector<unsigned char*> frames;
	
	// are we currently recording?
	bool recording;
	
	
	
	// how loud the microphone is.
	float inputLevel;
	
	
	ofxControlPanel gui;

	vector<VideoSample*> samples;
	int sampleInputPos;
	
	
	// parameters
	float recordThreshold;
	float recordStopThreshold;
	float minRecordTime;
	
	bool recordMode;
	int lastSamplePlayed;
	float playbackSpeed;
};

#endif	

