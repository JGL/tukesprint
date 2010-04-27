/*
 *  Sample.h
 *  openFrameworks
 */
#include <stdio.h>
#include <string>

#include "Sample.h"

#ifndef VIDEO_SAMPLE
#define VIDEO_SAMPLE



using namespace std;
class VideoSample: public Sample {
	
public:
	
	VideoSample();
	void loadFrames(unsigned char **_frames, int _frameCount);
	~VideoSample() {
		//delete [] data;
	};
	void dealloc() {
#ifndef SHALLOWCOPY 
		delete [] data;
#endif
	}
	unsigned char *getFrame();

	int frameCount;

private:
	int framePos;
	unsigned char **frames;
	
	
};
#endif

