#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "modules/camera/cameraconfig.h"
#include "osutils.h"
#include "string.h"
#include "logger.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "linux/videodev2.h"
#include <opencv2/opencv.hpp>
#include <cstdio>
const char* cameraconfig_defaultconfig=
"<cameras>\n"
"<camL>\n"
"<dev>/dev/video0</dev>"
"</camL>\n"
"<camR>\n"
"<dev>/dev/video1</dev>"
"</camR>\n"
"</cameras>";

const char* cameraconfig_brightness = "brightness";
const char* cameraconfig_contrast = "contrast";
const char* cameraconfig_saturation = "saturation";
const char* cameraconfig_hue = "hue";
const char* cameraconfig_exposure = "exposure";
const char* cameraconfig_gain = "gain";
const char* cameraconfig_value = "value";
const char* cameraconfig_auto = "auto";
const char* cameraconfig_ctrl = "ctrl";
const char* cameraconfig_file="cfg/camera.cfg";
const char* cameraconfig_left="camL";
const char* cameraconfig_right="camR";
const char* cameraconfig_devname = "dev";
void cameraconfig::readconfig(cameraconfig & cfg, int mid){
	char buff[8192];
	long size = 8192;
	cfg.m_id=mid;
	readFile(cameraconfig_file,buff,size);
	if(size==0){
		size = strlen(cameraconfig_defaultconfig);
		writeFile(cameraconfig_file,cameraconfig_defaultconfig,size);
		readconfig(cfg,mid);
		return;
	}
	using namespace rapidxml;
	xml_document<> doc;
	doc.parse<0>(buff);
	xml_node<> * body = doc.first_node();
	xml_node<> * cam_left = body->first_node(cameraconfig_left,strlen(cameraconfig_left));
	xml_node<> * cam_right = body->first_node(cameraconfig_right,strlen(cameraconfig_right));
	if(cam_left==NULL){
		Logger::log(mid, LOGGER_ERROR, "Camera config: No node for left camera!");
	}else{
		xml_node<> * dev = cam_left->first_node(cameraconfig_devname,strlen(cameraconfig_devname));
		if(dev==NULL){
			Logger::log(mid,LOGGER_ERROR,"Camera config: No node for device name in left camera!");
		}else{
			strcpy(cfg.dev_cam_left,dev->value());
			cfg.cam_left_enabled=1;	
			cfg.camconfig(cfg.dev_cam_left,cam_left,cfg.left_settings);
			//	read calibration params 
			const char* CM = cam_left->first_node("CM")->value(),
				*D = cam_left->first_node("D")->value(),
				*R = cam_left->first_node("R")->value(),
				*P = cam_left->first_node("P")->value();
			cfg.alg_params->m_left.CM=loadMatrix(CM);
			cfg.alg_params->m_left.D=loadMatrix(D);
			cfg.alg_params->m_left.R=loadMatrix(R);
			cfg.alg_params->m_left.P=loadMatrix(P);
			
		}	
	}
	if(cam_right==NULL){
		Logger::log(mid, LOGGER_ERROR, "Camera config: No node for right camera!");
	}else{
		xml_node<> * dev = cam_right->first_node(cameraconfig_devname,strlen(cameraconfig_devname));
		if(dev==NULL){
			Logger::log(mid,LOGGER_ERROR,"Camera config: No node for device name in right camera!");
		}else{
			strcpy(cfg.dev_cam_right,dev->value());	
			cfg.cam_right_enabled=1;	
			cfg.camconfig(cfg.dev_cam_right,cam_right,cfg.right_settings);
			//	read calibration params 
			const char* CM = cam_right->first_node("CM")->value(),
				*D = cam_right->first_node("D")->value(),
				*R = cam_right->first_node("R")->value(),
				*P = cam_right->first_node("P")->value();
			cfg.alg_params->m_right.CM=loadMatrix(CM);
			cfg.alg_params->m_right.D=loadMatrix(D);
			cfg.alg_params->m_right.R=loadMatrix(R);
			cfg.alg_params->m_right.P=loadMatrix(P);
		}
	}	
	
	cfg.loadAlgorithms(body);
	
}
void setparam(rapidxml::xml_node<>* node, int i){
	char buff[256];
	sprintf(buff,"%d",i);
	node->value(node->document()->allocate_string(buff));
}

void setparam(rapidxml::xml_node<>* node, float i){
	char buff[256];
	sprintf(buff,"%f",i);
	node->value(node->document()->allocate_string(buff));
}
void setparam(rapidxml::xml_node<>* node, double i){
	char buff[256];
	sprintf(buff,"%f",i);
	node->value(node->document()->allocate_string(buff));
}
void cameraconfig::saveconfig(){
	using namespace rapidxml;
	char buff[8192];
	long size = 8192;
	readFile(cameraconfig_file,buff,size);
	xml_document<> doc;
	doc.parse<0>(buff);
	xml_node<> * body = doc.first_node();
	//Here i assume that everything exists. no fall back if things dont exist. FIX THIS
	xml_node<> * camL = body->first_node("camL");
	xml_node<> * camR = body->first_node("camR");
	xml_node<> * algorithm = body->first_node("algorithm");
	//save matricies
	saveMatrix(alg_params->m_left.CM,camL->first_node("CM")->value());
	saveMatrix(alg_params->m_left.D,camL->first_node("D")->value());
	saveMatrix(alg_params->m_left.R,camL->first_node("R")->value());
	saveMatrix(alg_params->m_left.P,camL->first_node("P")->value());
	saveMatrix(alg_params->m_right.CM,camR->first_node("CM")->value());
	saveMatrix(alg_params->m_right.D,camR->first_node("D")->value());
	saveMatrix(alg_params->m_right.R,camR->first_node("R")->value());
	saveMatrix(alg_params->m_right.P,camR->first_node("P")->value());
	
	saveMatrix(alg_params->m_calib.SM.map1x,algorithm->first_node("SM_M1X")->value());
	saveMatrix(alg_params->m_calib.SM.map1y,algorithm->first_node("SM_M1Y")->value());
	saveMatrix(alg_params->m_calib.SM.map2x,algorithm->first_node("SM_M2X")->value());
	saveMatrix(alg_params->m_calib.SM.map2y,algorithm->first_node("SM_M2Y")->value());
	saveMatrix(alg_params->m_calib.SM.E,algorithm->first_node("SM_E")->value());
	saveMatrix(alg_params->m_calib.SM.F,algorithm->first_node("SM_F")->value());
	saveMatrix(alg_params->m_calib.SM.Q,algorithm->first_node("SM_Q")->value());
	
	// save algorithm parameters
	xml_node<> *pattern_width = algorithm->first_node("pattern_width"),
		*pattern_height = algorithm->first_node("pattern_height"),
		*num_frames = algorithm->first_node("num_frames"),
		*imagedelay = algorithm->first_node("imagedelay"),
		*LD_THRESH = algorithm->first_node("LD_THRESH"),
		*LD_GAUSS = algorithm->first_node("LD_GAUSS"),
		*LD_GAUSS_KERNEL = algorithm->first_node("LD_GAUSS_KERNEL"),
		*LD_CANNY_THRESH1 = algorithm->first_node("LD_CANNY_THRESH1"),
		*LD_CANNY_THRESH2 = algorithm->first_node("LD_CANNY_THRESH2"),
		*LD_HOUGH_RHO = algorithm->first_node("LD_HOUGH_RHO"),
		*LD_HOUGH_THETA = algorithm->first_node("LD_HOUGH_THETA"),
		*LD_HOUGH_THRESH = algorithm->first_node("LD_HOUGH_THRESH"),
		*LD_HOUGH_MIN_LINE = algorithm->first_node("LD_HOUGH_MIN_LINE"),
		*LD_HOUGH_MAX_GAP = algorithm->first_node("LD_HOUGH_MAX_GAP");
	setparam(pattern_width, alg_params->m_calib.pattern_size.width);
	setparam(pattern_height, alg_params->m_calib.pattern_size.height);
	setparam(num_frames, alg_params->m_calib.num_frames);
	setparam(imagedelay, alg_params->m_calib.image_delay);
	setparam(LD_THRESH, alg_params->m_line.initial_thresh);
	setparam(LD_GAUSS, alg_params->m_line.gauss_blur);
	setparam(LD_GAUSS_KERNEL, alg_params->m_line.gauss_blur_kernel_size);
	setparam(LD_CANNY_THRESH1, alg_params->m_line.canny_thresh1);
	setparam(LD_CANNY_THRESH2, alg_params->m_line.canny_thresh2);
	setparam(LD_HOUGH_RHO, alg_params->m_line.hough_rho);
	setparam(LD_HOUGH_THETA, alg_params->m_line.hough_theta);
	setparam(LD_HOUGH_THRESH, alg_params->m_line.hough_thresh);
	setparam(LD_HOUGH_MIN_LINE, alg_params->m_line.hough_min_line);
	setparam(LD_HOUGH_MAX_GAP, alg_params->m_line.hough_max_gap);
	char* end = print(buff,doc,0);
	*end =0;	
	writeFile(cameraconfig_file,end,strlen(end));
	Logger::log(m_id,LOGGER_INFO,"Saved camera configuration.");
}


cv::Mat cameraconfig::loadMatrix(const char* filename){
	using namespace cv;
	FileStorage fs(filename,FileStorage::READ);
	Mat output;
	if(fs.isOpened()){
		fs["MAT"] >> output;
		fs.release();
	}
	return output; 
}

void cameraconfig::saveMatrix(cv::Mat& img, const char* filename){
	using namespace cv;
	FileStorage fs(filename,FileStorage::WRITE);
	if(fs.isOpened()){
		fs <<"MAT" << img;
		fs.release();
	}
		
}

void cameraconfig::loadAlgorithms(rapidxml::xml_node<>* cam){
	using namespace rapidxml;
	//load algorithm
	xml_node<> * algorithm = cam->first_node("algorithm");
	//load calib parameters
	int w = atoi(algorithm->first_node("pattern_width")->value()),
		h= atoi(algorithm->first_node("pattern_height")->value());
	alg_params->m_calib.pattern_size = cv::Size(w,h);
	alg_params->m_calib.num_frames = atoi(algorithm->first_node("numframes")->value());
	alg_params->m_calib.image_delay = atoi(algorithm->first_node("imagedelay")->value());
	const char * SM_M1X = algorithm->first_node("SM_M1X")->value(),
		*SM_M1Y = algorithm->first_node("SM_M1Y")->value(),
		*SM_M2X = algorithm->first_node("SM_M2X")->value(),
		*SM_M2Y = algorithm->first_node("SM_M2Y")->value(),
		*SM_E   = algorithm->first_node("SM_E")->value(),
		*SM_F   = algorithm->first_node("SM_F")->value(),
		*SM_Q   = algorithm->first_node("SM_Q")->value();
	//load matricies
	alg_params->m_calib.SM.map1x = loadMatrix(SM_M1X);	
	alg_params->m_calib.SM.map1y = loadMatrix(SM_M1Y);	
	alg_params->m_calib.SM.map2x = loadMatrix(SM_M2X);	
	alg_params->m_calib.SM.map2y = loadMatrix(SM_M2Y);
	alg_params->m_calib.SM.E = loadMatrix(SM_E);
	alg_params->m_calib.SM.F = loadMatrix(SM_F);
	alg_params->m_calib.SM.Q = loadMatrix(SM_Q);
	
	//load line detector config
	
	alg_params->m_line.initial_thresh = atoi(algorithm->first_node("LD_THRESH")->value());
	alg_params->m_line.gauss_blur = atof(algorithm->first_node("LD_GAUSS")->value());
	alg_params->m_line.gauss_blur_kernel_size = atoi(algorithm->first_node("LD_GAUSS_KERNEL")->value());
	alg_params->m_line.canny_thresh1 = atoi(algorithm->first_node("LD_CANNY_THRESH1")->value());
	alg_params->m_line.canny_thresh2 = atoi(algorithm->first_node("LD_CANNY_THRESH2")->value());
	alg_params->m_line.hough_rho = atof(algorithm->first_node("LD_HOUGH_RHO")->value());
	alg_params->m_line.hough_theta = atof(algorithm->first_node("LD_HOUGH_THETA")->value());
	alg_params->m_line.hough_thresh = atoi(algorithm->first_node("LD_HOUGH_THRESH")->value());
	alg_params->m_line.hough_min_line = atof(algorithm->first_node("LD_HOUGH_MIN_LINE")->value());
	alg_params->m_line.hough_max_gap = atof(algorithm->first_node("LD_HOUGH_MAX_GAP")->value());
	
}

void cameraconfig::checkCtrl(cam_ctrl& camctrl,int fd, int ID){
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control ctrl;
	memset(&queryctrl,0,sizeof(queryctrl));
	queryctrl.id=ID;
	if(ioctl(fd,VIDIOC_QUERYCTRL, &queryctrl)==-1 || queryctrl.flags & V4L2_CTRL_FLAG_DISABLED){
		camctrl.available=false;	
		camctrl.value=0;
	}
	else{
		camctrl.available=true;
		memset(&ctrl,0,sizeof(ctrl));
		ctrl.id = ID;
		camctrl.min=queryctrl.minimum;	
		camctrl.max=queryctrl.maximum;
		if(ioctl(fd,VIDIOC_G_CTRL,&ctrl)==-1){
			camctrl.value = queryctrl.default_value;
		}else{
			camctrl.value = ctrl.value;
		}
	}

}

void cameraconfig::checkAuto(cam_ctrl& camctrl,int fd, int ID){
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control ctrl;
	memset(&queryctrl,0,sizeof(queryctrl));
	queryctrl.id=ID;
	if(ioctl(fd,VIDIOC_QUERYCTRL, &queryctrl)==-1 || queryctrl.flags & V4L2_CTRL_FLAG_DISABLED){
		camctrl.canauto=false;	
		camctrl.onauto=false;
	}
	else{
		camctrl.canauto=true;
		memset(&ctrl,0,sizeof(ctrl));
		ctrl.id = ID;
		if(ioctl(fd,VIDIOC_G_CTRL,&ctrl)==-1){
			camctrl.onauto = queryctrl.default_value;
		}else{
			camctrl.onauto = ctrl.value;
		}
	}

}

void cameraconfig::readCtrl(cam_ctrl& ctrl, rapidxml::xml_node<> *cam, const char* ctrlname){
	using namespace rapidxml;
	xml_node<> *ctrlnode = cam->first_node(ctrlname, strlen(ctrlname));
	if(ctrlnode==NULL)return;
	xml_node<> *valuenode = ctrlnode->first_node(cameraconfig_value,strlen(cameraconfig_value));
	if(valuenode!=NULL)
		ctrl.value=atoi(valuenode->value());
	xml_node<> *autonode = ctrlnode->first_node(cameraconfig_auto,strlen(cameraconfig_auto));
	if(autonode!=NULL)
		ctrl.onauto = atoi(autonode->value());	
}

void cameraconfig::configCtrl(cam_ctrl& camctrl, int fd, int id, int autoid){	
	struct v4l2_control ctrl;	
	if(camctrl.available){
		memset(&ctrl,0,sizeof(ctrl));
		ctrl.id = id;
		ctrl.value = camctrl.value;
		if(ioctl(fd,VIDIOC_S_CTRL,&ctrl)==-1)
		{
			Logger::log(m_id,LOGGER_ERROR,"failed to set ctrl :(");
		}
	}
	if(camctrl.canauto){
		memset(&ctrl,0,sizeof(ctrl));
		ctrl.id =autoid;
		ctrl.value = camctrl.onauto;;
		if(ioctl(fd,VIDIOC_S_CTRL,&ctrl)==-1)
		{
			Logger::log(m_id,LOGGER_ERROR,"failed to set ctrl :(");
		}
	}
}

void cameraconfig::readCustomCtrls( rapidxml::xml_node<> * cam, cam_settings & settings){
	settings.num_custom =0;
	using namespace rapidxml;
	xml_node<> * node = cam->first_node(cameraconfig_ctrl);
	int nCtrls=0;
	while(node){
		xml_node<> *name = node->first_node("name"),
				*value = node->first_node("value");
		if(!name || !value){
			Logger::log(m_id,LOGGER_ERROR,"");
		}
		settings.custom[nCtrls].name = new char[strlen(name->value())];	
		settings.custom[nCtrls].value = atoi(value->value());
		strcpy(settings.custom[nCtrls].name,name->value());
		node = node->next_sibling(cameraconfig_ctrl);	
		nCtrls++;
		//std::cout << "read in "<<settings.custom[nCtrls].name;
	}
	settings.num_custom=nCtrls;
}

void cameraconfig::configCustomCtrls(int fd, cam_settings& settings){
	struct v4l2_queryctrl queryctrl;
	memset(&queryctrl,0,sizeof(queryctrl));
	for(queryctrl.id = V4L2_CID_PRIVATE_BASE;;queryctrl.id++){
		if(0==ioctl(fd,VIDIOC_QUERYCTRL,&queryctrl)){
			if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				continue;
			for(int i=0;i<settings.num_custom;i++){
				if(0==strcmp((const char*)queryctrl.name,settings.custom[i].name)){
					struct v4l2_control control;
					memset(&control,0,sizeof(control));
					control.id = queryctrl.id;
					control.value = settings.custom[i].value;
					if(-1 == ioctl(fd,VIDIOC_S_CTRL,&control)){
						Logger::log(m_id,LOGGER_ERROR,"Camera config: Unable to setup custom ctrl.");
					}
					break;
				}
			}
		}else
			break;
	}
}

void cameraconfig::camconfig(const char* dev, rapidxml::xml_node<> *cam, cam_settings& settings){
	int fd = open(dev, O_RDWR);
	if(fd==-1)
	{
		Logger::log(m_id,LOGGER_ERROR,"Unable to open camera for config:");
		Logger::log(m_id,LOGGER_ERROR,dev);
		return;
	}

	//	Get Standard Hardware Capabilities
	checkCtrl(settings.brightness	, fd, V4L2_CID_BRIGHTNESS	);
	checkAuto(settings.brightness	, fd, V4L2_CID_AUTOBRIGHTNESS	);
	checkCtrl(settings.contrast	, fd, V4L2_CID_CONTRAST		);	
	settings.contrast.canauto=settings.contrast.onauto=false;
	checkCtrl(settings.saturation	, fd, V4L2_CID_SATURATION	);
	settings.saturation.canauto=settings.saturation.onauto=false;
	checkCtrl(settings.hue		, fd, V4L2_CID_HUE	 	);
	checkAuto(settings.hue		, fd, V4L2_CID_HUE_AUTO	 	);
	checkCtrl(settings.exposure	, fd, V4L2_CID_EXPOSURE 	);
	settings.exposure.canauto=settings.exposure.onauto=false;
	checkCtrl(settings.gain		, fd, V4L2_CID_GAIN	 	);
	checkAuto(settings.gain		, fd, V4L2_CID_AUTOGAIN		);
	
	//	Read Config File
	readCtrl(settings.brightness	, cam, cameraconfig_brightness	);	
	readCtrl(settings.contrast	, cam, cameraconfig_contrast	);	
	readCtrl(settings.saturation	, cam, cameraconfig_saturation	);	
	readCtrl(settings.hue		, cam, cameraconfig_hue		);	
	readCtrl(settings.exposure	, cam, cameraconfig_exposure	);	
	readCtrl(settings.gain		, cam, cameraconfig_gain	);	
	
	//	Read Custom Controls
	readCustomCtrls(cam, settings);

	//	Configure Hardware
	configCtrl(settings.brightness, fd, V4L2_CID_BRIGHTNESS, V4L2_CID_AUTOBRIGHTNESS);
	configCtrl(settings.contrast, fd, V4L2_CID_CONTRAST, 0);
	configCtrl(settings.saturation, fd, V4L2_CID_SATURATION, 0);
	configCtrl(settings.hue, fd, V4L2_CID_HUE, V4L2_CID_HUE_AUTO);
	configCtrl(settings.exposure, fd, V4L2_CID_EXPOSURE, 0);
	configCtrl(settings.gain, fd, V4L2_CID_GAIN, V4L2_CID_AUTOGAIN);
	configCustomCtrls(fd,settings);	
	close(fd);
	
}

