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
#include "linux/videodev2.h"

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
	char buff[4096];
	long size = 4096;
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
		}
	}	
	

	
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

