#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,240);
	#else
        vidPlayer.loadMovie("fingers.mov");
        vidPlayer.play();
	#endif

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	RImage_pixels = new RImage<float>(320, 240);
	fo = NULL;
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
        #endif

        grayImage = colorImg;
        int totalDetectionPixels = 320*240;
        unsigned char * detectionPixels = grayImage.getPixels();
        for (int i = 0; i < totalDetectionPixels; i++){
			RImage_pixels->array[i] = detectionPixels[i];
		}

        detectBlink(320, 240, (*RImage_pixels));
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetColor(0xffffff);
	colorImg.draw(20,20);
	grayImage.draw(360,20);

	if(!fo) return;

	ofNoFill();

	ofRect( fo->x+ 20 , fo->y+20, fo->xSize, fo->ySize);

	int eyeSize = static_cast<int>(fo->xSize * 0.1);

	if(fo->activation<-20)
		ofFill();
	else
		ofNoFill();

	//lefteye
	ofRect( fo->eyes.xLeft - eyeSize/2.0 + 20,
			fo->eyes.yLeft - eyeSize/2.0 + 20,
			eyeSize,
			eyeSize);

	//righteye
	ofRect( fo->eyes.xRight - eyeSize/2.0 + 20,
			fo->eyes.yRight - eyeSize/2.0 + 20,
			eyeSize,
			eyeSize);
	//ofRect(700,20,30,ofMap(smileThreshold,0,1,0,200));
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	//vidGrabber.videoSettings();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

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

