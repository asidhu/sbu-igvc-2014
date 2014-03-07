#ifndef _ARDUINO_MODULE_H
#define _ARDUINO_MODULE_H
#include "module.h"
#include "modules/arduino/arduinodata.h"
#include <iostream>





class arduinomodule:public module{
private:
	static const char* myName;
	static void* thread(void* arg);
	static void printEvent(std::ostream&, const event*);
	volatile bool m_dataArrived;
	void initializeReader();
	arduinodata m_data;
public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	arduinomodule(){
		m_dataArrived=false;
	}

};


#endif
