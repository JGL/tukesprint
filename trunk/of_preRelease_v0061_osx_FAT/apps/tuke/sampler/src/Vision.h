/*
 *  Vision.h
 *  sampler
 *
 *  Created by Marek Bereza on 27/04/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#include "ofxControlPanel.h"

#include "ofMain.h"
#include "ofxOpenCv.h"

#define VISION_WIDTH 640
#define VISION_HEIGHT 480
class Vision {
public:
	ofVideoGrabber camera;
	ofxCvColorImage camImg;
	ofxCvGrayscaleImage greyImg;
	ofxCvGrayscaleImage bgImg;
	ofxCvGrayscaleImage diffImg;
	ofxCvGrayscaleImage blendImage;
	
	ofxControlPanel *gui;
	
	vector<float> levels;
	bool vertical;
	void setup(ofxControlPanel *_gui) {
		vertical = true;
		gui = _gui;
		camera.initGrabber(VISION_WIDTH, VISION_HEIGHT);
		camImg.allocate(VISION_WIDTH, VISION_HEIGHT);
		greyImg.allocate(VISION_WIDTH, VISION_HEIGHT);
		bgImg.allocate(VISION_WIDTH, VISION_HEIGHT);
		diffImg.allocate(VISION_WIDTH, VISION_HEIGHT);
		blendImage.allocate(VISION_WIDTH, VISION_HEIGHT);
		
		bgImg.set(0);
		
		gui->setWhichPanel("vision");
		gui->setWhichColumn(0);
		gui->addDrawableRect("video", &camera, 320, 240);
		gui->addDrawableRect("diffImage", &diffImg, 200, 150);
		gui->addSlider("Learn Speed", "learnSpeed", 0.5, 0, 1, false);
		gui->addSlider("Number of Notes", "levels", 5, 5, 30, true);
		gui->addToggle("Vertical or Horizontal", "vertical", vertical);
	}
	
	void update() {
		camera.grabFrame();
		if(camera.isFrameNew()) {
			

			float learnSpeed = gui->getValueF("learnSpeed");
			
			camImg.setFromPixels(camera.getPixels(), VISION_WIDTH, VISION_HEIGHT);
			
			greyImg = camImg;
			blendImage = greyImg;
			
			// weighted scale
			cvConvertScale(bgImg.getCvImage(), bgImg.getCvImage(), 1-learnSpeed);
			cvConvertScale(blendImage.getCvImage(), blendImage.getCvImage(), learnSpeed);
			
			// mix two images
			cvAdd(bgImg.getCvImage(), blendImage.getCvImage(), bgImg.getCvImage());
			
			diffImg.absDiff(greyImg, bgImg);

			levels.clear();
			int numLevels = gui->getValueI("levels");
			vertical = gui->getValueB("vertical");
			unsigned char *pixels = diffImg.getPixels();
			for(int i = 0; i < numLevels; i++) {
				ofRectangle rect;
				if(vertical) {
					rect.x = 0;
					rect.width = VISION_WIDTH;
					rect.height = VISION_HEIGHT/numLevels;
					rect.y = rect.height*i;
				} else {
					rect.height = VISION_HEIGHT;
					rect.y = 0;
					rect.width = VISION_WIDTH/numLevels;
					rect.x = rect.width*i;
				}
				levels.push_back(getAveragePixelValueInRect(pixels, &rect));
			}
			
//			camera.getPixels();
		}
	}
	
		
	float getAveragePixelValueInRect(unsigned char *pixels, ofRectangle *rect) {
		float total = 0;
		
		for(int x = rect->x; x < rect->x + rect->width; x++) {
			for(int y = rect->y; y < rect->y + rect->height; y++) {
				float val = pixels[(int)(x + y*rect->width)];
				val /= 255.f;
				total += val;
			}
		}
		return total/(rect->width*rect->height);
	}
	
	void draw() {
		
		// additive blending
		glBlendFunc(GL_ONE, GL_ONE);
		ofSetColor(0xFF7777);

		diffImg.draw(0, 0, ofGetWidth(), ofGetHeight());
		diffImg.draw(0, 0, ofGetWidth(), ofGetHeight());
		diffImg.draw(0, 0, ofGetWidth(), ofGetHeight());
		
		// reset to normal alpha blending
		ofEnableAlphaBlending();
		/*
		for(int i = 0; i < levels.size(); i++) {
			ofSetColor(100, 255, 100, 50);
			ofRect(0, (float)i*ofGetHeight()/levels.size(), ofGetWidth()*levels[i], ofGetHeight()/levels.size());
		}*/
	}
};