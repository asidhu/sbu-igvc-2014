#ifndef _MOTOR_MODULE_H
#define _MOTOR_MODULE_H
#include "module.h"
#include "modules/motors/motorctrl.h"
class motormodule:public module{
	private:
	char dev_name[256];
	static const char* myName;
	volatile bool running;
	public:
	motorctrl m_ctrl;	
	static void* thread(void* args);
	void manageMotors();
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}

};


#endif
