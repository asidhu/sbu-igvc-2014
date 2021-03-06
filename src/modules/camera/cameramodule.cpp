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
		int leftCam = atoi(module->m_cfg.dev_cam_left);
		int rightCam = atoi(module->m_cfg.dev_cam_right);
		cv::VideoCapture m_left(leftCam),m_right(rightCam);
		if(!m_left.isOpened()){
			Logger::log(module->m_moduleid,LOGGER_ERROR,"Unable to open left camera!");
		}
		if(!m_right.isOpened()){
			Logger::log(module->m_moduleid,LOGGER_ERROR,"Unable to open right camera!");
		}
		module->m_parameters->calibration_mode = CALIBRATE_LINE_DETECTOR;
		module->running=true;
		while(module->running){
			module->runCamera(&m_left, &m_right);
		}
		m_left.release();
		m_right.release();	
		return NULL;			
	}


	
	void cameramodule::runCamera(cv::VideoCapture* L, cv::VideoCapture* R){
		using namespace cv;
		VideoCapture left=*L,
			right = *R;
		Mat grass = imread("grass2.jpg",CV_LOAD_IMAGE_COLOR);
		//Mat leftPic,rightPic;
		//leftPic=rightPic=grass;
		left.grab();
		Mat tst;
		left.retrieve(tst);
			
		VideoWriter leftSave("log/left.mp4",CV_FOURCC('X','2','6','4'),24,Size(320,240)), 
			rightSave("log/right.mp4",CV_FOURCC('X','2','6','4'),24,Size(320,240)),
			leftDSave("log/leftDetected.mp4",CV_FOURCC('X','2','6','4'),24,Size(320,240)), 
			rightDSave("log/rightDetected.mp4",CV_FOURCC('X','2','6','4'),24,Size(320,240));
		while(running){	
		if(!left.isOpened() || !right.isOpened())
			return;
		left.grab();
		right.grab();
		Mat leftPic,rightPic;
		if(!left.retrieve(leftPic)||
			!right.retrieve(rightPic))
			return;
		resize(leftPic,leftPic,Size(320,240));	
		resize(rightPic,rightPic,Size(320,240));	
		if(m_parameters->m_left.calibrated){
		
			stereomap SM = m_parameters->m_calib.SM;
			remap(leftPic.clone(),leftPic,SM.map1x,SM.map1y,INTER_LINEAR,BORDER_CONSTANT,Scalar());
		}
		if(m_parameters->m_right.calibrated){
			stereomap SM = m_parameters->m_calib.SM;
			remap(rightPic.clone(),rightPic,SM.map2x,SM.map2y,INTER_LINEAR,BORDER_CONSTANT,Scalar());
		}
		m_parameters->m_left.normal=leftPic;
		m_parameters->m_right.normal=rightPic;
		cvtColor(leftPic, m_parameters->m_left.gray ,CV_BGR2GRAY);
		cvtColor(rightPic,m_parameters->m_right.gray ,CV_BGR2GRAY);
		split(leftPic,m_parameters->m_left.channels);
		split(rightPic,m_parameters->m_right.channels);
		Mat blob,mask,lines;
		m_algorithms->objectDetection(leftPic.clone(),blob,mask);
		cvtColor(blob,blob,CV_GRAY2BGR);
		cvtColor(mask,mask,CV_GRAY2BGR);
		m_algorithms->lineDetection(leftPic.clone(),lines);
	
		//imshow("input", leftPic-blob);
		imshow("l",leftPic+mask+lines);
		leftSave<<leftPic;
		leftDSave<<leftPic+mask+lines;	
	//m_algorithms->calib(calibration_parameters,leftPic,rightPic);
		m_algorithms->objectDetection(rightPic,blob,mask);
		cvtColor(blob,blob,CV_GRAY2BGR);
		cvtColor(mask,mask,CV_GRAY2BGR);
		//m_algorithms->lineDetection(rightPic,lines);
		imshow("r",rightPic+mask+lines);
		rightDSave<<rightPic+mask+lines;	
		rightSave<<rightPic;	
			

			
			waitKey(41);
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
		m_cfg.alg_params=m_parameters;
		cameraconfig::readconfig(m_cfg,m_moduleid);
		initializeReader();
		m_algorithms = new camera_algorithm();
		m_algorithms->beginCalib(m_parameters);
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
		if(evt->m_eventflag==EFLAG_TERMINATE)
			running=false;		
	}

const char* cameramodule::myName="Camera Module";
