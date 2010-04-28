/*
 *  Sample.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
#ifndef ABS
#define ABS(A) (A>0?A:-A)
#endif

#include "Sample.h"
Sample::Sample() {
	
	data = NULL;
}

void Sample::load(float* _data, int _length) {

	pos = 0;
	playing = false;
	length = _length;
	volume = 1.f;
#ifdef SHALLOWCOPY
	data = _data;
#else
	if(data!=NULL) delete data;
	data = new float[length];
	for(int i = 0; i < length; i++) {
		data[i] = _data[i];
	}
	
#endif
}

void Sample::trigger(float vel) {
	volume = vel;
	pos = 0;
	playing = true;
}

float Sample::getSample() {
	if(data==NULL) return 0;
	if(playing && pos<length && pos>=0) {
		return volume*data[(int)pos++];
	} else {
		playing = false;
		return 0;
	}

}

float Sample::getSample(float speed) {
	if(data==NULL) return 0;
	if(playing && pos+speed<length && pos>=0) {
		pos += speed;
		return volume*data[(int)pos];
	} else {
		playing = false;
		return 0;
	}
}

void Sample::getSamples(float* out, int _length) {
	if(data==NULL) return;
	int count = 0;
	for(int i = pos; i < _length+pos && i < length; i++) {
		if(i<3) {
			out[(int)(i-pos)] = ((float)(i+1.f)/3.f)*volume*data[i];
		} else if(i+3>length) {
			out[(int)(i-pos)] = ((float)(length - i)/3.f)*volume*data[i];
		} else {
			out[(int)(i-pos)] = volume*data[i];
		}
		count++;
	}
	pos += count;
	// if we've reached the end
	if(pos>=length-1) {
		playing = false;
		for(int i = count; i < _length; i++) {
			out[i] = 0.f;
		}
	}
}

void Sample::normalize() {
	if(data==NULL) return;
	float max = 0;
	for(int i = 0; i < length; i++) {
		if(ABS(data[i])>max) max = ABS(data[i]);
	}
	
	// don't want a divide by zero or any extra work
	if(max==0 || ABS(max)==1) {
		return ;
	}
	// what do we need to multiply max by to make it 1?
	float gain = 1.f/max;
	for(int i = 0; i < length; i++) {
		data[i] *= gain;
	}
}

void Sample::addSamples(float* out, int _length) {
	int count = 0;

	for(int i = pos; i < _length+pos && i < length; i++) {
		if(i<3) {
			out[(int)(i-pos)] += ((float)(i+1.f)/3.f)*volume*data[i];
		} else if(i+3>length) {
			out[(int)(i-pos)] += ((float)(length - i)/3.f)*volume*data[i];
		} else {
			out[(int)(i-pos)] += volume*data[i];
		}
		count++;
	}
	pos += count;
}

void Sample::stop() {
	playing = false;
	pos = 0;
}


