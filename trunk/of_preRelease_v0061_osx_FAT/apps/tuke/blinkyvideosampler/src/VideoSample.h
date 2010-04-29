/*
 *  Sample.h
 *  openFrameworks
 */
#include <stdio.h>
#include <string>
#include "ofMain.h"
#include "Sample.h"
#define VIDEO_WIDTH		320
#define VIDEO_HEIGHT	240
#pragma once

class VideoSample: public Sample {
	
public:
	
	VideoSample();
	void loadFrames(unsigned char **_frames, int _frameCount);
	~VideoSample() {
		deallocateVideo();
	};
	
	void deallocateVideo();
	void trigger(float volume);
	unsigned char *getFrame();
	
	int frameCount;
	void draw();
	void draw(float aWidth, float aHeight);
	
private:
	ofImage display;
	int framePos;
	unsigned char **frames;
	
};

