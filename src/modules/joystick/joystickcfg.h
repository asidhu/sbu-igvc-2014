#ifndef _JOYSTICK_CFG_H
#define _JOYSTICK_CFG_H


//cfg should store mapping
struct joystick_cfg{
	char dev_name[256];
	struct{
		char id;
		short min, max;
	} lt;
	struct{
		char id;
		short min, max;
	} rt;
	char safety,forward,rotate_left,rotate_right,backwards;



};


#endif


