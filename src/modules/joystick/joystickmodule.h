#ifndef _JOYSTICK_MODULE_H
#define _JOYSTICK_MODULE_H
#include "module.h"
#include "modules/joystick/joystickcfg.h"
class joystickmodule:public module{
	private:
	static const char* myName;
	joystick_cfg m_cfg;
	public:
	bool polling;
	int calibration;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	void pollEvents();
	void calibrateJoyStick(int jd_dev);
	static void* thread(void* args);
	const char* getCommonName(){
		return myName;
	}

};


#endif
