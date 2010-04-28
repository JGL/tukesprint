/*
 *  VideoSample.cpp
 *  openFrameworks
 *
 */

#include "VideoSample.h"
VideoSample::VideoSample() {
	Sample::Sample();
	frameCount = 0;
	framePos = 0;
	frames = NULL;
	display.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
}

void VideoSample::deallocate() {
	if(frames!=NULL) {
		for(int i = 0; i < frameCount; i++) {
			delete [] frames[i];
		}
		delete frames;
		frames = NULL;
	}
}

void VideoSample::trigger(float volume) {
	Sample::trigger(volume);
	framePos = 0;
}

void VideoSample::loadFrames(unsigned char** _frames, int _frameCount) {
	dealloc();
	frameCount = _frameCount;
	frames = _frames;
	printf("Recorded %d frames\n", frameCount);
}

unsigned char *VideoSample::getFrame() {
	if(frames==NULL || framePos>=frameCount) return NULL;
	unsigned char *toReturn = frames[framePos];
	framePos++;
	
	return toReturn;
}

void VideoSample::draw() {
	unsigned char *frame = getFrame();
	if(frame!=NULL) {
		display.setFromPixels(frame, VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR, true);
	}
	display.draw(0, 0);
	
}