#pragma once

#include "ofMain.h"
#include "ofxMSAFluid.h"
#include "ParticleSystem.h"
#include "ofxControlPanel.h"
#include "customDrawer.h"
#include "contourF.h"
#include "ofxOpenCv.h"


#define USE_GUI		



class testApp : public ofSimpleApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();

	void windowResized(int w, int h);

	void addToFluid(float x, float y, float dx, float dy, bool addColor = true, bool addForce = true);
	
		
	customDrawer		colorPick;
	contourF			contDraw;

	int					fluidCellsX;
	bool				resizeFluid;
	bool				drawFluid;
	bool				drawParticles;
	bool				renderUsingVA;
	
	ofxMSAFluidSolver	fluidSolver;
	ofxMSAFluidDrawer	fluidDrawer;	
	
	ParticleSystem		particleSystem;
	
	int					pmouseX, pmouseY;
	
	// ----- OPEN CV STUFF
	ofVideoGrabber 		vidGrabber;
	ofxCvColorImage		colorImg;
	
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayBg;
	ofxCvGrayscaleImage 	grayDiff;
	
	ofxCvContourFinder 	contourFinder;
	
	int 				threshold;
	bool				bLearnBakground;
	
	
	
	
	// cache these for slightly better performance
	struct {
		int				width;
		int				height;
		float			invWidth;
		float			invHeight;
		float			aspectRatio;
		float			aspectRatio2;
	} window;
	
	
#ifdef USE_GUI 
	ofxControlPanel gui;

#endif
	
	
};

extern testApp *myApp;



