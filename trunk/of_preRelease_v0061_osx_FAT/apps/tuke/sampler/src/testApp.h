#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Sample.h"
#include "Vision.h"

#include "ofxControlPanel.h"

#define MAX_RECORD_SECONDS 10
#define SAMPLERATE 44100

class Particle {
public:
	ofPoint pos, vel;
	float age;
	bool alive;
	static ofImage particle;
	static ofxControlPanel *gui;
	static ofColor *color;
	float maxSize;
	float maxAge;
	Particle(float x = 0, float y = 0, float speed = 0, float angle = 0) {
		if(particle.width==0) {
			particle.loadImage("particle.png");
			particle.setAnchorPercent(0.5, 0.5);
		}
		alive = true;
		age = 0;
		pos.x = x;
		pos.y = y;
		speed *= gui->getValueF("starSpeed");
		vel.x = speed*cos(angle);
		vel.y = speed*sin(angle);
		maxSize = gui->getValueF("starSize");
		maxAge = gui->getValueF("starMaxAge");
	}
	void update() {
		age++;
		pos += vel;
	}
	
	void draw() {
		float radius = ofMap(age, 0, maxAge, maxSize, 0);
		float alpha = ofMap(age, 0, maxAge, 255, 0);
		if(radius<0) {
			alive = false;
			alpha = 0;
			radius = 0;
		}
		ofSetColor(color->r, color->g, color->b, alpha);
		particle.draw(pos.x, pos.y, radius, radius);
	}
};
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

	
	void spawnParticle(ofPoint pos, float volume);
	vector<Particle> particles;
	// takes care of all the vision
	Vision vision;
	
	ofxControlPanel gui;
	customDrawer particleColorPicker;
	float noteLastTime;
	int lastNote;
	vector<string> sounds;
	vector<string> scales;
	int lastSound;
};

#endif	

