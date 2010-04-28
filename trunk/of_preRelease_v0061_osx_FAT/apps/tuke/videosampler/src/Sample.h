/*
 *  Sample.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
#include <stdio.h>
#include <string>


#ifndef SAMPLE
#define SAMPLE

//#define SHALLOWCOPY 

using namespace std;
class Sample {
	
public:
	
	Sample();
	void load(float* data, int length);
	~Sample() {
		//delete [] data;
	};
	void dealloc() {
#ifndef SHALLOWCOPY 
		delete [] data;
#endif
	}
	virtual void trigger(float volume);
	void getSamples(float* out, int length);
	void addSamples(float* out, int length);
	float getSample();
	float getSample(float speed);
	void stop();
	void normalize();
	bool playing;
	
	int length;
	float *data;
	float volume;

private:
	float pos;
	
	
};
#endif

