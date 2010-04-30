
#pragma once
#include "ofMain.h"
#include "ofxVectorMath.h"
#include "ofxBox2d.h"
#include "Filter.h"
#include "ofxControlPanel.h"

#define TOPGRAPHICTYPETOPBORDER 40.f
#define TOPGRAPHICTYPEBOTTOMBORDER 30.f
#define MAXTIMEBETWEENPARTICLES 10.f
#define MINTIMEBETWEENPARTICLES 1.f
#define MAXPARTICLESIZE 2.f				//users can go up to twice as big as normal
#define MINPARTICLESIZE 0.5f			//or half...
#define NUMBEROFSHAKES	10
#define SCROLLMESSAGEDURATION 10000.f	//give the messages 10 seconds to scroll
#define SCROLLMESSAGEDELAY	500.f	//wait one second before a new message

#define LIFEOFPARTICLE 1.f //5 minute life span? 60*5 = 300

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle {
	
public:
	CustomParticle() {
	}
	//float timeOfBirth;
	//void setTimeOfBirth(float newBirthTime){
	//	timeOfBirth = newBirthTime;
//	}
	ofColor color;
	float myRadius;
	int flip;
//	void setColour(ofColor newColour){
//		colour = newColour;
//	}
	void setRadius() {
		myRadius=0;
		flip=1;
		
	}
	void draw() {
		float radius = getRadius();
	//	ofImage star;
		//star.loadImage("particle1.png");
		/*ofTexture starTexture;
		
		starTexture.allocate(star.width, star.height, GL_RGB, false);
		starTexture.loadData(star.getPixels(), star.width, star.height, GL_RGB);
		
		starTexture.bind();*/
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		//ofRotate((color.r));
		//starTexture.draw(0,0);
		//star.draw(0,0);
		ofSetColor(color.r, color.g, color.b);
		 ofFill();
		 ofCircle(0, 0, radius*3);	
		
		
		
		glPopMatrix();
		
	}
	
	void explode() {
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		ofSetColor(255, 25, 255,100);
		ofFill();
		ofCircle(0, 0, getRadius()+myRadius*flip);
		ofCircle(0, 0, (getRadius()+(myRadius*flip))/2);
		ofCircle(0, 0, (getRadius()+(myRadius*flip))/4);
		myRadius--;
		if(myRadius==-20){
			flip*=-1;	
		}
		//myRadius*fabs(ofRandom(0,3));
		glPopMatrix();
				
		//}
		
	}
	void update(int index) {
		
		
		
	}
	
};

// -------------------------------------------------

class testApp : public ofBaseApp {
	
public:
	
	testApp();
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	void audioReceived 	(float * input, int bufferSize, int nChannels); 
	void generateParticles(int height);
	void graph(int height, int x, int y);
	void judgeParticles();
	void eventsIn(guiCallbackData & data);
	
	ofxControlPanel gui;
	simpleFileLister lister;
	simpleLogger logger;
	ofImage image1;
	ofImage	image2;
	
	ofTrueTypeFont TTF;
	
	float px, py;
	bool							bDrawLines;
	bool							bMouseForce;
	
	ofxBox2d						box2d;			  //	the box2d world
	vector		<ofxBox2dCircle>	circles;		  //	default box2d circles
	vector		<ofxBox2dPolygon>	polygons;		  //	defalut box2d polgons
	vector		<ofxBox2dRect>		boxes;			  //	defalut box2d rects
	vector      <ofxBox2dLine>		lines;			  //	default box2d lines (hacked)
	vector		<CustomParticle*>	customParticles;  //	this is a custom particle the extends a cirlce
	
	ofxBox2dCircle					ballJoints[5];	  //	ball joints
	ofxBox2dJoint					joints[5];		  //	box2d joints
	
	ofxBox2dLine					lineStrip;		  //	a linestrip for drawing
	
	ofPoint upDirection;
	ofPoint upForce;
	
	float * left;
	float * right;
	int 	bufferCounter;
	int 	drawCounter;
	float screenH;
	float screenW;
	
	float elapsedTime;
	int appFrameCount;
	float appFrameRate;
	int lastParticleX;
	int lastParticleY;
	int lastPartX[10];
	int lastPartY[10];
	
	
	float myMean;
	float threshold;
	CFxRbjFilter filter;
	float cutoff;
	
	int red_colour;
	int green_colour;
	int blue_colour;
	
	float inputLevel;
	float filteredInputLevel;
	int startTime;
	
	
	void shrinkPop(float sausage, float banger, float gravy);
	
	
	
};

