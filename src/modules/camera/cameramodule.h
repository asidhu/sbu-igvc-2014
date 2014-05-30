#ifndef _CAMERA_MODULE_H
#define _CAMERA_MODULE_H
#include "module.h"
#include "modules/camera/cameradata.h"
#include "modules/camera/cameraconfig.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>



class cameramodule:public module{
private:
	static const char* myName;
	static void* thread(void* arg);
	static void printEvent(std::ostream&, const event*);
	void initializeReader();
	void runCamera(cv::VideoCapture*,cv::VideoCapture*);
	cameraconfig m_cfg;
public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	cameramodule(){
	
	}

};


#endif
