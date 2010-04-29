/*
 *  ofxTwitter.h
 *  openFrameworks
 *
 *  Created by Todd Vanderlin on 11/21/09.
 *  Copyright 2009 Interactive Design. All rights reserved.
 *
 */


#pragma once
#include "ofMain.h"

#include "ofxTweet.h"
#include "ofxTwitterProfile.h"
#include "ofxTwitterEvents.h"
#include "ofxTwitterRequest.h"

#include "ofxXmlSettings.h"
#include "ofxThread.h"


#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
	//JGL addition
#include "Poco/Net/HTTPBasicCredentials.h"

#include <iostream>
#include <vector>
#include <istream.h>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;
using Poco::BasicEvent;
using Poco::Delegate;
	//JGL addition
using Poco::Net::HTTPBasicCredentials;



// --------------------------------------------- URL Helpers
const string TWITTER_SCREEN_NAME   = "screen_name=";
const string TWITTER_USER_TIMELINE = "http://twitter.com/statuses/user_timeline.xml";
const string TWITTER_USER_PROFILE  = "http://twitter.com/users/show.xml";
const string TWITTER_USER_MENTIONS = "http://api.twitter.com/1/statuses/mentions.xml";


// --------------------------------------------- Type of twitter calls
enum {
	TWITTER_TYPE_STATUS,
	TWITTER_TYPE_USER,
	TWITTER_TYPE_MENTIONS
};



// --------------------------------------------- Twitter

class ofxTwitter : public ofxThread {
	
protected:
	
	
	ofxXmlSettings				xml;
	vector <ofxTwitterRequest>	request;
	int							requestID;
	int							timeoutSeconds;
	bool						bLoading;
	bool						bVerbose;
	
	
public:
	
	
	
	// --------------------------------------------- Events
	ofEvent <TwitterResponseArgs>	TWITTER_RESPONSE;
	ofEvent <TwitterErrorArgs>		TWITTER_ERROR;
	
	
	// --------------------------------------------- Twitter
	ofxTwitter() {
		bVerbose		= true;
		bLoading		= false;
		requestID		= 0;
		timeoutSeconds	= 10;
	}
	~ofxTwitter() {
		request.clear();
		xml.clear();
		stop();
	}
	
	// --------------------------------------------- allocate
	void allocate() {
		requestID = 1;
		start();
	}
	
	// --------------------------------------------- Start / Stop
	void start() {
		if(isThreadRunning() == false) {
			startThread(true, false);	
		}
	}
	void stop() {
		stopThread(); 
	}
	
	bool isLoading() {
		return bLoading;
	}
	
	// --------------------------------------------- Make a HTTP Request	
	void makeHTTPRequest(ofxTwitterRequest& httpReq) {
		
		if(httpReq.url.compare("") == 0) {
			if(bVerbose) printf("--- error no url: %i ---\n", httpReq.id);
			httpReq.bDone = true;
			return;
		}
		
		try {
			
			if(bVerbose) printf("--- making req: %i\n", httpReq.id);
			
			URI uri(httpReq.url.c_str());
			string path(uri.getPathAndQuery());
			if (path.empty()) path = "/";
			
			HTTPClientSession session(uri.getHost(), uri.getPort());
			HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
				//JGL addition, authentication required to get mentions
			if(httpReq.type == TWITTER_TYPE_MENTIONS){
				HTTPBasicCredentials creds("USERNAME", "PASSWORD"); //TODO: make this value settable externally
				creds.authenticate(req);				
			}
			session.sendRequest(req);
			
			session.setTimeout(Poco::Timespan(timeoutSeconds, 0));
			
			HTTPResponse res;
			istream& rs = session.receiveResponse(res);
			
			if(bVerbose) printf("--- req made: %i\n", httpReq.id);
			
			// now setup the response
			httpReq.setupResponse(res, rs);
			parseResponse(httpReq);
			
			
		} 
		
		catch (Exception& exc){
			if(bVerbose) printf("--- error %s ---\n", exc.displayText().c_str());
			httpReq.bDone = true;
		}
		
	}
	
	// --------------------------------------------- Parse the request
	void parseResponse(ofxTwitterRequest& req) {
		
		xml.clear();
		xml.loadFromBuffer(req.content);
		
		
		// -------------------------------------------------
		if(req.type == TWITTER_TYPE_STATUS) {
			
			bool isStatuesTag = xml.tagExists("statuses");
			if(isStatuesTag) {
				xml.pushTag("statuses");
				
				int numStatuses = xml.getNumTags("status");
				if(bVerbose) printf("Num Statuses: %i\n", numStatuses);
				
				if(numStatuses > 0) {
					for(int i=0; i<numStatuses; i++) {
						xml.pushTag("status", i);
						
						ofxTweet tweet;
						tweet.text = xml.getValue("text", "NULL");
						req.tweets.push_back(tweet);
						xml.popTag();
					}
				}
				
				xml.popTag();
			}
			
			// all set and parsed 
			req.bHasResponse = true;
		}
		
		// -------------------------------------------------
		if(req.type == TWITTER_TYPE_USER) {
			bool isUserTag = xml.tagExists("user");
			if(isUserTag) {
				printf("--- urser added ---\n");
				xml.pushTag("user");
				/*
				// string name = xml.getValue("screen_name", "none");
				ofxTwitterProfile prf;
				prf.id				  = req.id;
				prf.name			  = xml.getValue("name", "none");
				prf.screen_name		  = xml.getValue("screen_name", "none");
				prf.profile_image_url = xml.getValue("profile_image_url", "none");
				prf.followersCount    = xml.getValue("followers_count", -1);
				prf.friendsCount	  = xml.getValue("friends_count", -1);
				prf.statusCount		  = xml.getValue("statuses_count", -1);
				req.profiles.push_back(prf);
				*/
				xml.popTag();
			}
		}
		
			// -------------------------------------------------
		if(req.type == TWITTER_TYPE_MENTIONS) { // JGL - just the same
			
			bool isStatuesTag = xml.tagExists("statuses");
			if(isStatuesTag) {
				xml.pushTag("statuses");
				
				int numStatuses = xml.getNumTags("status");
				if(bVerbose) printf("Num Statuses: %i\n", numStatuses);
				
				if(numStatuses > 0) {
					for(int i=0; i<numStatuses; i++) {
						xml.pushTag("status", i);
						
						ofxTweet tweet;
						tweet.text = xml.getValue("text", "NULL");
						xml.pushTag("user");
						tweet.screen_name = xml.getValue("screen_name", "");
						xml.popTag();
						req.tweets.push_back(tweet);
						xml.popTag();
					}
				}
				
				xml.popTag();
			}
			
				// all set and parsed 
			req.bHasResponse = true;
		}		
		
		/*xml.pushTag("statuses");
		 int numStatuses = xml.getNumTags("status");
		 
		 //printf("Num:%i\n", numStatuses);
		 
		 if(numStatuses > 0) {
		 for(int i=0; i<numStatuses; i++) {
		 xml.pushTag("status", i);
		 string tweet = xml.getValue("text", "NULL");
		 //printf("[%i] Tweet:%s\n", i, tweet.c_str());
		 //tweets.push_back(ofxTweet());
		 //tweets.back().text = tweet;
		 xml.popTag();
		 }
		 }
		 */
		xml.clear();
	}		
	
	// --------------------------------------------- The Treaded Function
	void threadedFunction() {
		
		while (isThreadRunning() == true) {
			
			if(lock()) {
				
				bLoading = true;
				
				for(int i=request.size()-1; i>=0; i--) {
					
					ofxTwitterRequest& req = request[i];
					
					// make the request if not made yet
					if(req.bRequestMade == false) {
						req.bRequestMade = true;
						makeHTTPRequest(req);
					}
					
					// do we have a response and made a request
					if(req.bHasResponse == true && 
					   req.bRequestMade == true &&
					   req.bDone == false) {
						
						req.bDone = true;
						
						static TwitterResponseArgs responseArgs;
						if(request[i].type == TWITTER_TYPE_STATUS) {
							responseArgs.id		 = req.id;
							responseArgs.content = req.content;
							responseArgs.type    = req.type;
							responseArgs.tweets  = req.tweets;
						}
						if(request[i].type == TWITTER_TYPE_MENTIONS) {
							responseArgs.id		 = req.id;
							responseArgs.content = req.content;
							responseArgs.type    = req.type;
							responseArgs.tweets  = req.tweets;
						}					
						
						ofNotifyEvent(TWITTER_RESPONSE, responseArgs, this);
						
					}
				}	
				
				
				// clean up if the request is done
				bool bAllDoneLoadingTweets = true;
				for(int i=request.size()-1; i>=0; i--) {
					if(request[i].bDone) {
						//request.erase(request.begin() + i);
					}
					else {
						bAllDoneLoadingTweets = false;
					}
				}
				
				if(bAllDoneLoadingTweets) bLoading = false;
				
				
				// stop if there are no more request
				if(request.size() <= 0) {
					stop();
				}
			}
			
			// slowdown cowboy
			unlock();
			ofSleepMillis(100);
		}
		
	}
	
	// --------------------------------------------- Add Request
	void addRequest(ofxTwitterRequest &req) {
		if(bVerbose) printf("--- adding request: %i ---\n", req.id);
		request.push_back(req);
		if(isThreadRunning() == false) start();
	}
	
	// ---------------------------------------------
	int getUserTweets(string userName, int numTweets, int page=1) {
		
		// Turn on the thread
		if(isThreadRunning() == false) start();	
		
		// make a request and add it
		ofxTwitterRequest newRequest;
		newRequest.id   = requestID ++;
		newRequest.url  = TWITTER_USER_TIMELINE+"?"+TWITTER_SCREEN_NAME+userName+"&count="+ofToString(numTweets)+"&page="+ofToString(page);
		newRequest.type = TWITTER_TYPE_STATUS;
		
		// go make the request
		addRequest(newRequest);
		
		return newRequest.id;
	}
	
	// ---------------------------------------------	
	int getUserProfile(string userName, bool loadImage = true) {
		
		// Turn on the thread
		if(isThreadRunning() == false) start();	
		
		// make a request and add it
		ofxTwitterRequest newRequest;
		newRequest.id   = requestID ++;
		newRequest.url  = TWITTER_USER_PROFILE+"?"+TWITTER_SCREEN_NAME+userName;
		newRequest.type = TWITTER_TYPE_USER;
		
		// go make the request
		addRequest(newRequest);
		
		return newRequest.id;
		
		/*
		 ofxTwitterProfile profile;
		 bool loaded = twitter.userGet(userName);
		 
		 if(loaded) {
		 
		 string replyMsg;
		 twitter.getLastWebResponse( replyMsg );
		 printf( "Twitter Says:%s\n", replyMsg.c_str() );
		 
		 
		 xml.clear();
		 xml.loadFromBuffer(replyMsg);
		 int numTags = xml.tagExists("xml");
		 
		 string name = xml.getValue("user:name", "ad");
		 printf("num:%i\n", numTags);
		 
		 if(numTags > 0) {
		 xml.pushTag("user");
		 
		 profile.id = xml.getValue("id", -1);
		 profile.name = xml.getValue("name", "NULL");
		 profile.screen_name = xml.getValue("screen_name", "NULL");
		 profile.profile_url = xml.getValue("profile_image_url", "NULL");
		 profile.friendsCount = xml.getValue("friends_count", -1);
		 profile.followersCount = xml.getValue("followers_count", -1);
		 profile.statusCount = xml.getValue("statuses_count", -1);
		 
		 xml.popTag();
		 }
		 xml.clear();
		 
		 profile.print();
		 
		 // load the profile image
		 if(loadImage && numTags > 0) {
		 profile.loadProfileImage();
		 }
		 }
		 else {
		 printf("error loading profile\n");
		 }
		 return profile;
		 */
		
		return 0;
	}
	
		// ---------------------------------------------
		// JGL addition
	int getUserMentions(int numTweets=1, int page=1) {
		
			//all occuring as specc'd by http://apiwiki.twitter.com/Twitter-REST-API-Method%3A-statuses-mentions
		
			// Turn on the thread
		if(isThreadRunning() == false) start();	
		
			// make a request and add it
		ofxTwitterRequest newRequest;
		newRequest.id   = requestID ++;
		newRequest.url  = TWITTER_USER_MENTIONS+"?count="+ofToString(numTweets)+"&page="+ofToString(page);
		newRequest.type = TWITTER_TYPE_MENTIONS;
		
		cout << "The request url is: " << newRequest.url << " the type is " << newRequest.type << endl;
		
			// go make the request
		addRequest(newRequest);
		
		return newRequest.id;
	}	
	
	
	
	
};
