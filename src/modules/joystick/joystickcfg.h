#ifndef _JOYSTICK_CFG_H
#define _JOYSTICK_CFG_H
#define BUTTON_SAFETY			0x1
#define BUTTON_FORWARD			0x2
#define BUTTON_ROTATE_LEFT		0x3
#define BUTTON_ROTATE_RIGHT		0x4
#define BUTTON_BACKWARDS		0x5
#define BUTTON_AUTO1			0x7
#define BUTTON_AUTO2			0x8
#define AXIS_THROTTLE			0x6
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
	char safety,forward,rotate_left,rotate_right,backwards, auto1,auto2;


};


#endif


