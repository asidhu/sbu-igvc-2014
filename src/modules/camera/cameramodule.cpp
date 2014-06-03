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
		while(true){
			module->runCamera(&m_left, &m_right);
		}
		return NULL;			
	}


	
	void cameramodule::runCamera(cv::VideoCapture* L, cv::VideoCapture* R){
		using namespace cv;
		VideoCapture left=*L,
			right = *R;
		Mat grass = imread("grass.jpg",CV_LOAD_IMAGE_COLOR);
		
		m_parameters->m_left.normal=grass;
		m_parameters->m_right.normal=grass;
		m_algorithms->lineDetection(m_parameters);
		
			waitKey(30);

		if(!left.isOpened() || !right.isOpened())
			return;
		left.grab();
		right.grab();
		Mat leftPic,rightPic;
		if(!left.retrieve(leftPic)||
			!right.retrieve(rightPic))
			return;
		
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

		//m_algorithms->calib(calibration_parameters,leftPic,rightPic);
		
			

			
			waitKey(30);
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
		
	}

const char* cameramodule::myName="Camera Module";
