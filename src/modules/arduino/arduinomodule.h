#ifndef _ARDUINO_MODULE_H
#define _ARDUINO_MODULE_H
#include "module.h"
#include "modules/arduino/arduinodata.h"
#include <iostream>
#include <vector>
#include <cstring>




class arduinomodule:public module{
private:
	static const char* myName;
	char cfgfile[256];
	char path[256];
	static void* thread(void* arg);
	static void printEvent(std::ostream&, const event*);
	volatile bool m_dataArrived;
	void initializeReader();
	void readArduino();
	int m_device;
	volatile bool running;
	std::vector<arduinodata*> m_events, m_sent_events, m_recycler;
	std::vector<arduinocmd*> m_cmds;
	arduinodata* getArduinoData(){
		if(m_recycler.empty())
			return new arduinodata();
		else{
			arduinodata* out = m_recycler.back();
			m_recycler.pop_back();
			return out;
		}
	}
	static void findDev(char *path);
	static bool devExists(char *path);
	static std::vector<int> inUse;
  public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	
	arduinomodule(const char *cfgfile){
		running=true;
	        strcpy(this->cfgfile, cfgfile);
		m_dataArrived=false;
	}

};



#endif
