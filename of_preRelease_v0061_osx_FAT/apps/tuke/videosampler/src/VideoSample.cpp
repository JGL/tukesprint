/*
 *  VideoSample.cpp
 *  openFrameworks
 *
 */

#include "VideoSample.h"
VideoSample::VideoSample() {
	frameCount = 0;
	frames = NULL;
}

void VideoSample::loadFrames(unsigned char** _frames, int _frameCount) {
	frameCount = _frameCount;
	frames = _frames;
}

unsigned char *VideoSample::getFrame() {
	if(frames==NULL) return NULL;
	/*
	if(playing && pos<length && pos>=0) {
		return volume*data[(int)pos++];
	} else {
		playing = false;
		return 0;
	}
	 */
	
	return NULL;
}

