/*
 *  customDrawer.h
 *  ofxControlPanelDemo
 *
 *  Created by theo on 02/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "guiCustomImpl.h"
#include "guiBaseObject.h" //theo added for isInsideRect()
#include "ofxXmlSettings.h"

class customDrawer : public guiCustomImpl {
	
private:
	
	vector < vector <ofPoint> > pts;
	
public:
	
	bool mouseIsRelative;
	ofImage colorwheel;
	ofColor col;
	
	int markerX, markerY;
	
	customDrawer(){
		colorwheel.loadImage("colorwheel.png");

	}
	string createXMLName() {
		int pos = 0;

	
		string str = name;
		string old = " ";
		string _new = "_";
		while((pos = str.find(old, pos)) != std::string::npos) {
			str.replace(pos, old.length(), _new);
			pos += _new.length();
		}
		return str;
	}
	// -------------------------------------
	void saveSettings(string filename){
		ofxXmlSettings xml;
		xml.loadFile("color"+filename);
		string tagName = createXMLName();
		xml.addTag(tagName);
		xml.pushTag(tagName);
		xml.addTag("val_0");
		xml.addTag("val_1");
		xml.addTag("val_2");
		xml.setValue("val_0", col.r);
		xml.setValue("val_1", col.g);
		xml.setValue("val_2", col.b);
		xml.saveFile("color"+filename);
	}
	
	void reloadSettings(){
	
	}
	
	void saveSettings(){
	
	}
	
	void loadSettings(string filename) {
		ofxXmlSettings xml;
		xml.loadFile("color"+filename);
		string tagName = createXMLName();
		col.r = xml.getValue(tagName+":val_0", 255);
		col.g = xml.getValue(tagName+":val_1", 255);
		col.b = xml.getValue(tagName+":val_2", 255);
		
		
		xml.saveFile("color"+filename);
	}
	
	// -------------------------------------
	float getWidth() {
		return boundingRect.width;
	}
	float getHeight() {
		return boundingRect.height;
	}
	
	// -------------------------------------
	void clearPoints() {
	
	}
	
	// -------------------------------------
	void addPoint(float x, float y) {

	}
	

	void draw(float x, float y, float w, float h) {
		colorwheel.draw(x, y, w, h);
		printf("%f %f\n", w, h);
		ofNoFill();
		ofSetColor(0, 0, 0);
		ofRect(x+markerX, y+markerY, 5, 5);
		ofFill();
		ofSetColor(col.r, col.g, col.b);
		ofRect(x+markerX+1, y+markerY+1, 3, 3);

	}
	
	// -------------------------------------
	void mousePressed(float x, float y) {

		if( isInsideRect(x, y, ofRectangle(0, 0, boundingRect.width, boundingRect.height) ) ){
			//cout << x << " " << y << endl;
			changeColor(x, y);	

		}
	}

	void changeColor(float x, float y){
		
		markerX = (int)x;
		markerY = (int)y;
		
		x *= (float)colorwheel.width/boundingRect.width;
		y *= (float)colorwheel.height/boundingRect.height;
		unsigned char* pixels = colorwheel.getPixels();
		int pos = (((int)y * colorwheel.width) + (int)x)*4;
		col.r = pixels[pos];
		col.g = pixels[pos+1];
		col.b = pixels[pos+2];
		
		

	}
	
	//these values come in with 0,0 being the top left of your bounding rect 
	// -------------------------------------
	void mouseDragged(float x, float y){

		//theo changed
		
		if( isInsideRect(x, y, ofRectangle(0, 0, boundingRect.width, boundingRect.height) ) ){
			changeColor(x, y); 
		}
	}
	
	void mouseReleased(float x, float y){
		if( isInsideRect(x, y, ofRectangle(0, 0, boundingRect.width, boundingRect.height) ) ){
			changeColor(x, y); 
		}
	}
	
};