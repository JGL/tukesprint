
#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp() {	
}

//--------------------------------------------------------------
void testApp::setup() {
	
	//filter cutoff freq
	cutoff = 1000.f;
	
	ofEnableAlphaBlending();
	image1.loadImage("ourjoey.jpg");
	image2.loadImage("fish.jpg");
	screenH=900;
	screenW=1440;
	
	//GUI SETUP
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 60, 200));
	ofxControlPanel::setTextColor(simpleColor(240, 50, 50, 255));
	
	gui.loadFont("MONACO.TTF", 8);		
	gui.setup("test cv", 0, 0, 480, 280);
	gui.addPanel("sensitivity", 4, false);
	gui.addPanel("background colour", 4, false);
	
	//choose panel and column
	gui.setWhichPanel(0);
	gui.setWhichColumn(0);
	
	gui.addButtonSlider("threshold", "BG_THRESHOLD", 0.2, 0.1, 1.9, false);
	gui.addButtonSlider("mass", "MASS", 3, 1, 20, false);
	gui.addButtonSlider("bounce", "BOUNCE", 1, 1, 20, false);
	gui.addButtonSlider("friction", "FRICTION",  0, 1, 20, false);
	
		
	
	gui.setWhichPanel(1);
	gui.setWhichColumn(1);
	
	gui.addButtonSlider("red_colour", "BG_COLOUR1", 0, 0, 255, true);
	gui.addButtonSlider("green_colour", "BG_COLOUR2", 0, 0, 255, true);
	gui.addButtonSlider("blue_colour", "BG_COLOUR3", 0, 0, 255, true);
	
	gui.setWhichPanel(1);
	gui.setWhichColumn(2);
	vector <string> names;
	names.push_back("blank");
	names.push_back("joey");
	names.push_back("fish");
	
	gui.addTextDropDown("difference mode", "DIFF_MODE", 0, names);
	
	
	//vector <guiVariablePointer> vars;
	
	//load from xml!
	gui.loadSettings("controlPanelSettings.xml");
	
	//if you want to use events call this after you have added all your gui elements
	gui.setupEvents();
	gui.enableEvents();

	ofAddListener(gui.guiEvent, this, &testApp::eventsIn);
	///////////////END OF GUI STUFF

	//amplitutde of input
	inputLevel = 0;
	
	//amplitude of high freq
	filteredInputLevel = 0;
	
	ofSetVerticalSync(true);
	ofBackground(20, 20, 20);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofToggleFullscreen();
	bDrawLines  = false;
	bMouseForce = false;
	
	box2d.init();
	box2d.setGravity(0, 20);
	
	//this call is creating the floor at the bottom of the window; not the screen!?
	//box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	ofSoundStreamSetup(0,2,this, 44100, 256, 1);	
	//left = new float[256];
	//right = new float[256];
	
	
	bufferCounter = 0;
	drawCounter = 0;	
	lineStrip.setWorld(box2d.getWorld());
	lineStrip.clear();
	
	lineStrip.addPoint(0,screenH);
	lineStrip.addPoint(screenW,screenH);
	lineStrip.createShape();
	//setting up the upDirection and upForce
	upDirection.x = 20;
	upDirection.y = 20;
	upForce.x = 20;
	upForce.y = 10;
	
	//threshold=0.27;
	
}
void testApp::eventsIn(guiCallbackData & data){
	
	
	 ///we use the event callback to capture the background - we then set the toggle value back to its previous value
	/* if( data.groupName == "GRAB_BACKGROUND" && data.getFloat(0.0) == 1.0 ){
	 bgExample.captureBackground();
	 gui.setValueB("GRAB_BACKGROUND", false);
	 }*/
	 
	//lets send all events to our logger
	if( data.groupName != "events logger"){
		string logStr = data.groupName;
		
		if( data.fVal.size() ){
			for(int i = 0; i < data.fVal.size(); i++) logStr += " - "+ofToString(data.fVal[i], 4);
		}
		if( data.iVal.size() ){
			for(int i = 0; i < data.iVal.size(); i++) logStr += " - "+ofToString(data.iVal[i]);
		}	
		if( data.sVal.size() ){
			for(int i = 0; i < data.sVal.size(); i++) logStr += " - "+data.sVal[i];
		}
		
		logger.log(OF_LOG_NOTICE, "event - %s", logStr.c_str());
	}
		startTime=0;
}


//--------------------------------------------------------------
void testApp::update() {
	
	gui.update();
	
	
	red_colour=gui.getValueI("BG_COLOUR1");
	green_colour=gui.getValueI("BG_COLOUR2");
	blue_colour=gui.getValueI("BG_COLOUR3");
	
	cout<<gui.getValueI("DIFF_MODE")<<" DIFFMODE \n";
	ofBackground(red_colour, green_colour, blue_colour);
	
	box2d.update();
	
	//generateParticles(820);
	generateParticles(screenH-80);
	//erase old particles from the vector
	//customParticles[0].
/*	if(ofGetFrameNum()>1){
	cout<<customParticles[1]->getPosition().x<<" customParticle x \n";
	cout<<customParticles[1]->getPosition().y<<" customParticle y \n";
	lastParticleX= customParticles[1]->getPosition().x;
	lastParticleY= customParticles[1]->getPosition().y;
	}*/
		
	if(customParticles.size()>500){
		
		//delete customParticles[0];
		//lastParticleX= customParticles[0]->getPosition().x;
		//lastParticleY= customParticles[0]->getPosition().y;
		
		

		customParticles.erase(customParticles.begin() );
		customParticles[0]->destroyShape();
		
	}
	
	filter.calc_filter_coeffs(1,cutoff,2,0,false);
	//judgeParticles();
	threshold=gui.getValueF("BG_THRESHOLD");
		if(ofGetFrameNum()%20==0){
			//cout<<threshold<<"\n";
			
	}

			for (int i=0; i<customParticles.size(); i++){
				//customParticles[i]->setPhysics(gui.getValueF("MASS"),gui.getValueF("BOUNCE"),gui.getValueF("FRICTION"));
				cout << gui.getValueF("MASS") << " mass\n";

				if(inputLevel>threshold){

			//customParticles[i]->setPhysics(gui.getValueI("MASS"),gui.getValueI("BOUNCE"),gui.getValueI("FRICTION"));
			customParticles[i]->addAttractionPoint((ofGetWidth()/2,0,0), 0, 1.0f, 1);
			customParticles[i]->addImpulseForce(upDirection.y, upForce.y);
			//customParticles[i]->addImpulseForce(upDirection.y, inputLevel*10);
			
		}
	}
}



//--------------------------------------------------------------
void testApp::draw() {
	if(gui.getValueI("DIFF_MODE")==1){
		image1.draw(0,0, screenW, screenH);
	   }
	if(gui.getValueI("DIFF_MODE")==2){
		image2.draw(0,0, screenW, screenH);
	}
	
	
	gui.draw();
	
	for(int i=0; i<customParticles.size(); i++) {
		customParticles[i]->draw();
	}
	
	px = mouseX;
	py = mouseY;
	
	
	//dont start until the particles start dying anyway
	
	if(ofGetFrameNum()>500){
		for(int i=1;i<30;i++){
			customParticles[i]->explode();
		}
	}
	

}

//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	
	
	float max = 0;
	float maxFiltered = 0;
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		
		//get max value from buffer
		float value = MAX(ABS(input[i*2]),ABS(input[i*2+1]));
		if(inputLevel<value) inputLevel = value;
		
		//if its not peaking reduce level by a bit
		else inputLevel *= 0.9995;
		
		float filteredOutput = filter.filter((input[i*2]+input[i*2+1])*0.5);
		if(filteredInputLevel<filteredOutput) filteredInputLevel = filteredOutput;
		else filteredInputLevel *= 0.9995;
	}
	bufferCounter++;
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	//setting the threshold...
	if(key == 'k') {
		threshold+=0.2;
	}
	if(key == 'm') {
		threshold-=0.2;
	}
	if(key == 'f') {
		ofToggleFullscreen();
	}
	
	}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
		gui.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	gui.mousePressed(x, y, button);

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(bDrawLines) lineStrip.createShape();
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
	
}

void testApp::generateParticles(int height){
	float r = ofRandom(3, 10);		// a random radius 4px - 20px
	CustomParticle *p = new CustomParticle();
	//p.setPhysics(0.4, 0.53, 0.31);
	p->setPhysics(20.9, 0.53, 0);
	
	p->setRadius();
	//p.setup(box2d.getWorld(), ofRandom(0,ofGetWidth()),2*(ofGetHeight()/3), r);
	p->setup(box2d.getWorld(), ofRandom(0,screenW),height, r);
	p->color.r = ofRandom(20, 100);
	p->color.g = 0;
	p->color.b = ofRandom(150, 255);
	customParticles.push_back(p);
	
}

void testApp::graph(int x, int y,int height){
	ofSetColor(10, 0, 200);
	
	ofRect(x, y, 30, -1*height);
	
	
}

/*void testApp::shrinkPop(float circX, float circY, float radiusBurst){
	
	float startTime = ofGetSeconds();
	float endTime = ABS(ofGetSeconds()+20.0f);
	if (endTime == startTime){
		ofSetColor(255, 255, 255);
		ofFill();

		ofCircle(circX, circY, radiusBurst);
	}

	
	
}*/

void testApp::shrinkPop(float circX, float circY, float radiusBurst){
	
	//period in frames for animation to run
	int period=20;
	bool myBool;
	int circsize;
	int size;
	
	if(ofGetFrameNum()%period==0){
		myBool=true;
		circsize=0;
		size=0;
		cout<<myBool<<" BOOOOOOOOOOLLL\n";
	}
		cout<<myBool<<" BOOOOOOOOOOLLL\n";
		
	if(myBool==true){
		startTime=ofGetFrameNum();
		circsize=1;
		myBool=false;
	}
	
	
	if(ofGetFrameNum()-startTime<=period){
		
		//while (period >= 0){
		ofFill();
		ofSetColor(255, 255, 255);
		//ofCircle(circX, circY, radiusBurst+size);
		//circsize++;
		size++;
	//	cout<<size<<" size\n";
		//}
	}
	//cout<<circsize<<"START TIME \n";
}
