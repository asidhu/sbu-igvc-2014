#include "modules/camera/cameramodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include "event_flag.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <linux/videodev2.h>
#include "logger.h"
	void* cameramodule::thread(void* args){
		cameramodule* module = (cameramodule*) args;
		cv::VideoCapture m_left(module->m_cfg.dev_cam_left),m_right(module->m_cfg.dev_cam_right);
		if(!m_left.isOpened()){
			Logger::log(module->m_moduleid,LOGGER_ERROR,"Unable to open left camera!");
		}
		if(!m_right.isOpened()){
			Logger::log(module->m_moduleid,LOGGER_ERROR,"Unable to open right camera!");
		}
		while(true){
			module->runCamera(&m_left, &m_right);
		}
		return NULL;			
	}
	
	void cameramodule::runCamera(cv::VideoCapture* L, cv::VideoCapture* R){
		using namespace cv;
		VideoCapture cap = *R;
		if(!cap.isOpened())
			return;
		Mat edges;
		Mat blue;
		namedWindow("edges",1);
		for(;;){
			Mat frame;
			if(!cap.read(frame))
				continue;
			Mat channel[3];
			split(frame,channel);
			blue=channel[0];
			GaussianBlur(channel[0],edges,Size(7,7),1.5,1.5);
			threshold(edges,edges,100,255,0);
			
			threshold(blue,blue,200,255,0);
			Mat skel(blue.size(), CV_8UC1, Scalar(0));
			Mat temp;
			Mat eroded;
			 
			Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
			 
			bool done;		
			
			do
			{
				erode(blue, eroded, element);
				dilate(eroded, temp, element); // temp = open(img)
				subtract(blue, temp, temp);
				bitwise_or(skel, temp, skel);
				eroded.copyTo(blue);
				done = (countNonZero(blue) == 0);
			} while (!done);
			imshow("skeleton",skel);
			Canny(edges,edges,0,30,3);
			//GaussianBlur(edges,edges,Size(7,7),1.5,1.5);
			std::vector<Vec4i> lines;
			HoughLinesP(edges,lines,1,CV_PI/180,50,50,10);
			
/*
			for(size_t i =0; i<lines.size();i++){
				float rho = lines[i][0], theta = lines[i][1];
				Point pt1, pt2;
				double a =cos(theta) , b=sin(theta);
				double x0 = a*rho, y0 = b*rho;
				pt1.x= cvRound(x0+1000*(-b));
				pt1.y= cvRound(y0+1000*(a));
				pt2.x= cvRound(x0-1000*(-b));
				pt2.y= cvRound(y0-1000*(a));
				line(edges,pt1,pt2,Scalar(255),3,CV_AA);
			}
*/
			for(size_t i=0;i<lines.size();i++){
				Vec4i v = lines[i];
				line(edges, Point(v[0],v[1]),Point(v[2],v[3]),Scalar(255),3,CV_AA);
			}
			imshow("edges",edges);
			imshow("regular",frame);
			
			if(waitKey(30)>=0)break;
		}
	}
	

	void cameramodule::initializeReader(){
		spawnThread(cameramodule::thread, this);
	}
	/**
		rules of module ettiquette:
		1) only create a thread if you are reading from a file, doing io actions, socket ops, sleeping, blocking OR intensive calculations being performed. (AKA camera manipulation operations, intense geometric operations, anything greater than 5 ms worth of time)
		2) all threads should be monitored and kept track of and cleaned up when asked to.
		3) all modules should respond to standard commands.
	**/
	
	/**
		Rules:
		1) FEEL FREE to read information in from files. 
		2) If this is like the camera, and a command comes in to connect to camera, dont read configuration than. Load configuration NOW.
		3) dont open hardware or pipes or sockets now, there is a command for that. Open it at that time, only read files.
		4) listeners should also be setup HERE. If you want to listen to some event, take listener_flag and | it with the event flag.
	**/
	void cameramodule::initialize(uint32& listener_flag){
		cameraconfig::readconfig(m_cfg,m_moduleid);
		initializeReader();
	}

	void cameramodule::printEvent(std::ostream& out, const event* evt){
	
	}
	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void cameramodule::update(bot_info* data){
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/	
	void cameramodule::pushEvent(event* evt){
		
	}

const char* cameramodule::myName="Camera Module";
