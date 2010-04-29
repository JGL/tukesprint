#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"

#include "blink.h"
#include <deque>

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file


class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		bool setupSmileDetector(int camWidth , int camHeight);
		void detectBlink(int camWidth, int camHeight,  const RImage<float> &pixels);

        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif

        ofxCvColorImage		colorImg;

        ofxCvGrayscaleImage 	grayImage;
        RImage<float>*      RImage_pixels;
        unsigned char *     detectionPixels; // detection pixels

        MPBlink blink;
        VisualObject        faces;

        FaceObject *fo;
};

#endif
