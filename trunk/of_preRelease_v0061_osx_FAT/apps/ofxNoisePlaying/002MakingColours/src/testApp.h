#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofAddons.h"

#include "ofxNoise.h"


class testApp : public ofSimpleApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
		//
	
	ofxPerlin *noise;
	
	ofTexture 		texColor;
	
	int 			w, h;
	
	unsigned char 	* colorPixels;
};

#endif