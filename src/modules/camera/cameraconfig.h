#ifndef _CAMERA_CONFIG_H
#define _CAMERA_CONFIG_H
#include "rapidxml/rapidxml.hpp"
struct cam_ctrl{
	char* name;
	int id;
	int min,max,value;
	bool available,canauto,onauto;
};

struct cam_settings{
	cam_ctrl brightness,contrast,saturation,hue,exposure,gain;
	cam_ctrl custom[256];
	int num_custom;	
	bool vFlip, hFlip;
};


class cameraconfig{
private:
	void camconfig(const char* dev, rapidxml::xml_node<> * cam, cam_settings &);
	void checkCtrl(cam_ctrl& ctrl, int fd, int ID);
	void checkAuto(cam_ctrl& ctrl, int fd, int ID);
	void readCtrl(cam_ctrl& ctrl, rapidxml::xml_node<> * cam, const char* ctrlname);
	void configCtrl(cam_ctrl& ctrl, int fd, int ID, int AUTOID);
	void readCustomCtrls( rapidxml::xml_node<> * cam, cam_settings & );
	void configCustomCtrls(int fd,cam_settings & );
public:
	int m_id;
	static void readconfig(cameraconfig& cfg, int mid);
	char dev_cam_left[256];
	char dev_cam_right[256];
	int cam_left_enabled,cam_right_enabled;
	cam_settings left_settings, right_settings;
};



#endif
