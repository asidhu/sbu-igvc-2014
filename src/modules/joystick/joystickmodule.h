#ifndef _JOYSTICK_MODULE_H
#define _JOYSTICK_MODULE_H
#include "module.h"
#include "modules/joystick/joystickcfg.h"
#include <vector>
#include <iostream>

class event;

class joystickevent{
	public:
	int type;
	union{
		int btnValue;
		double axisValue;
	};

};

class joystickmodule:public module{
	private:
	static const char* myName;
	void sendBtnEvt(int TYPE, int value);
	void sendAxisEvt(int TYPE, double value);
	static void printevent(std::ostream&, const event*);
	joystick_cfg m_cfg;
	std::vector<joystickevent*> m_event_pool;
	std::vector<joystickevent*> m_event_queue;
	std::vector<joystickevent*> m_event_sent;
	joystickevent* getEvent(){
		if(m_event_pool.size()==0)
			m_event_pool.push_back(new joystickevent());
		joystickevent* ret = m_event_pool.back();
		m_event_pool.pop_back();
		return ret;
	}
	volatile bool running;
	public:
	int calibration;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	void pollEvents();
	void loadConfig();
	void saveConfig();
	void calibrateJoyStick(int jd_dev);
	static void* thread(void* args);
	const char* getCommonName(){
		return myName;
	}
};


#endif
