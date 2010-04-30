#include "testApp.h"
#include "stdio.h"




//--------------------------------------------------------------
void testApp::setup(){	 
	recordMode = true;
	ofBackground(0,0,0);	
	
	recordBufferSize = SAMPLERATE*MAX_RECORD_SECONDS;
	recordBuffer = new float[recordBufferSize];
	recordPos = 0;
	recording = false;
	inputLevel = 0;
	sampleInputPos = 0;
	lastSamplePlayed = -1;
	playbackSpeed = 2.5;
	
	recordThreshold = 0.2;
	recordStopThreshold = 0.2;
	minRecordTime = 0.5f;
	
	
	memset(recordBuffer, 0, recordBufferSize*sizeof(float));
	// 1 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 1 num buffers (latency)
	ofSoundStreamSetup(2,1,this, SAMPLERATE, 256, 1);	
	ofSetFrameRate(25.f);
	
	
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 60, 200));
	ofxControlPanel::setTextColor(simpleColor(240, 50, 50, 255));
	
	gui.loadFont("isocpeur.ttf", 12);		
	gui.setup("Sampler", 0, 0, 500, 400);
	gui.addPanel("background subtraction example", 4, false);
		
	//--------- PANEL 1

	
	
	gui.setWhichColumn(1);
	gui.addSlider("Movement Sensitivity", "volumeSensitivity", 0.1, 0, 1, false);
	gui.addSlider("Movement Threshold", "movementThreshold", 0.1, 0, 1, false);
	//gui.addToggle("Loop Sound", "loopSound", loopSound);
	//gui.addSlider("Loop Duration", "loopDuration", 1.f, 0, 2, false);
	gui.addSlider("Pitch", "pitch", 0, -20, 20, true);
	//load from xml!
	gui.loadSettings("controlPanelSettings.xml");
	
	//if you want to use events call this after you have added all your gui elements
	gui.setupEvents();
	gui.enableEvents();	
	gui.hide();
	
	cam.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT);
	
		//blink detect by joel
	
	colorImg.allocate(VIDEO_WIDTH,VIDEO_HEIGHT);
	grayImage.allocate(VIDEO_WIDTH,VIDEO_HEIGHT);
	RImage_pixels = new RImage<float>(VIDEO_WIDTH, VIDEO_HEIGHT);
	fo = NULL;

}
void testApp::exit() {
	gui.saveSettings("controlPanelSettings.xml");
}

void testApp::update() {
	cam.grabFrame();
	
    bool bNewFrame = false;
	
	bNewFrame = cam.isFrameNew();
	
	if (bNewFrame){
		
		colorImg.setFromPixels(cam.getPixels(), VIDEO_WIDTH,VIDEO_HEIGHT);
		
        grayImage = colorImg;
        int totalDetectionPixels = VIDEO_WIDTH*VIDEO_HEIGHT;
        unsigned char * detectionPixels = grayImage.getPixels();
        for (int i = 0; i < totalDetectionPixels; i++){
			RImage_pixels->array[i] = detectionPixels[i];
		}
		
        detectBlink(VIDEO_WIDTH, VIDEO_HEIGHT, (*RImage_pixels));
	}	
	
	if(recording) {
		// capture frame if there's space
		if(frames.size()<MAX_NUM_FRAMES) {
			unsigned char *frame = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT*3];
			unsigned char *pixels = cam.getPixels();
			memcpy(frame, pixels, VIDEO_WIDTH*VIDEO_HEIGHT*3);
			frames.push_back(frame);
		}
	} else {

	}
	gui.update();
	
	
	if(recordMode) {
		// maybe trigger recording
		if(!recording) {

//			if(inputLevel>recordThreshold) {
//				printf("Start recording because %f > %f\n", inputLevel, recordThreshold);
//				startRecording();
//			}
			
				//not using audio triggering any more, just triggering via blinks
			
			if(fo){
					//we have a non null face object
				if(fo->activation < -20){
					startRecording();
				}
			}
				
		} else {
			// stop recording if the sound falls below the threshold and exceeded the
			// minimum record time, or we've exceeded the maximum record time.
//			if(recordPos>minRecordTime*44100 && (inputLevel<recordStopThreshold || recordPos>=recordBufferSize)) {
//				stopRecording();
//			}
			
				// stop recording if we get another blink and exceeded the
				// minimum record time, or we've exceeded the maximum record time.
			
				//cout << "Record pos " << recordPos << " fo " << fo << " fo activation " << fo->activation << endl;
			
			if((recordPos>minRecordTime*44100) && ((fo && (fo->activation < -20))|| recordPos>=recordBufferSize)) {
				stopRecording();
			}			
		}
	} else {
		if(samples.size()>0) {
			int sampleToPlay = ((int)(ofGetElapsedTimef()*playbackSpeed)) % samples.size();
			if(sampleToPlay!=lastSamplePlayed) {
				samples[sampleToPlay]->trigger(1);
				lastSamplePlayed = sampleToPlay;
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){	
	ofEnableAlphaBlending();

	cam.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	float drawWidth = ofGetWidth()/3.f;
	float drawHeight = (drawWidth / 4.f ) * 3.f; //keeping the proportions right
	
	int sampleRef = 0;
	
	for(int i = 0; i < 3; i++){
		for(int j = 0; j <3;  j++){
			glPushMatrix();
			glTranslatef(i*drawWidth, j*drawHeight, 0);
			if(sampleRef < samples.size()){
				samples[sampleRef]->draw(drawWidth, drawHeight);				
			} 
			glPopMatrix();
			sampleRef++;
		}
	}

	ofDisableAlphaBlending();
	gui.draw();
	if(recording) {
		ofSetColor(0xFF0000);
		ofRect(0, ofGetHeight()-20, ofGetWidth(), 20);
		ofSetColor(0xFFFFFF);
	}
}






void testApp::audioRequested (float * output, int bufferSize, int nChannels) {
	
	// if we're recording, we want silence!!
	if(recording) {
		memset(output, 0, bufferSize*nChannels*sizeof(float));
	} else {
		// otherwise, maybe we want playback
		for(int i = 0; i < bufferSize; i++) {
			float s = 0;
			for(int j = 0; j < samples.size(); j++) {
				s += samples[j]->getSample(1);
			}
			for(int channel = 0; channel < nChannels; channel++) {
				output[i*nChannels + channel] = s;
			}
		}
	}
}
//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	


	for(int i = 0; i < bufferSize; i++) {
		
		float inp = input[i*nChannels];
		
		// do the recording
		if(recording && recordPos<recordBufferSize) {
			recordBuffer[recordPos++] = inp;
		}
		// do a level meter
		if(inputLevel<inp) {
			inputLevel = inp;
		} else {
			inputLevel *= 0.99995;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if(key=='h') gui.toggleView();
	else if(key=='f') ofToggleFullscreen();
	//else startRecording();
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
	//stopRecording();
}


void testApp::startRecording() {
	if(!recording) { // this stops that weird key repeating thing
		recordPos = 0;
		recording = true;
		
		// dealloc any frames that might be in the buffer
		for(int i = 0; i < frames.size(); i++) {
			delete [] frames[i];
		}
		frames.clear();
	}
}

void testApp::stopRecording() {
	recording = false;
	VideoSample *sample = new VideoSample();
	sample->load(recordBuffer, recordPos);
	//	printf("We have %d samples\n", recordPos);
	//	float ave = 0;
	//	for(int i = 0; i < recordPos; i++) {
	//		ave += recordBuffer[i];
	//	}
	//	printf("Ave %f\n", ave/recordPos);
	unsigned char **frameArray = new unsigned char *[frames.size()];
	for(int i = 0; i < frames.size(); i++) {
		frameArray[i] = frames[i];
	}
	
	sample->loadFrames(frameArray, frames.size());
	sample->normalize(0.5);
	
	// add the sample to the list if there are less than 8
	if(samples.size()<MAX_VIDEOS) {
		sampleInputPos = samples.size();
		samples.push_back(sample);
		
		// replace a sample if there are 8
	} else {
		
		sampleInputPos++;
		sampleInputPos %= MAX_VIDEOS;
		delete samples[sampleInputPos];
		samples[sampleInputPos] = sample;
	}
	
	// don't deallocate memory, just forget the pointer
	// because VideoSample does a shallow copy.
	frames.clear();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	playbackSpeed = (float)x*8.f/ofGetWidth();
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}


//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	gui.mousePressed(x, y, button);
	for(int i = 0; i < samples.size(); i++) {
		samples[i]->trigger(1.f);
	}

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	recordMode ^= true;
	if(recording) stopRecording();
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::detectBlink(int camWidth, int camHeight,  const RImage<float> &pixels)
{
		//faces.clear();
	blink.findBlinks(pixels, faces); // mpisearch.search(pixels, faces, 1);
	if(faces.size() != 0) {
		float maxSize=0;
		while(!faces.empty( ))
		{
				//Square face = faces.front();
			VisualObject* face = faces.front();
			cout << "openness: "
			<< face->activation;
			if(face->activation > 5)
				cout << ", predicting open ";
			if(face->activation < -5)
				cout << ", predicting closed ";
			cout << endl;
			if(face->xSize>maxSize){
				fo = static_cast<FaceObject*>(face);
				maxSize=face->xSize;
			}
			
			faces.pop_front();
		}
		
	}	
	
}