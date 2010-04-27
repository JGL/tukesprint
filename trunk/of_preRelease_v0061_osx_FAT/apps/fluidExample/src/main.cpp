
#include "testApp.h"

testApp *myApp;
int main( ){
	ofSetupOpenGL(1440, 900, OF_FULLSCREEN);			// <-------- setup the GL context
	myApp = new testApp;
	ofRunApp(myApp);
}
