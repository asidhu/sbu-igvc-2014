#ifndef _GPS_MODULE_H
#define _GPS_MODULE_H
#include "module.h"
#include "modules/gps/gpsdata.h"
#include <iostream>





class gpsmodule:public module{
private:
	static const char* myName;
	static void printEvent(std::ostream&, const event*);
	gpsdata m_gpsdata;
public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	gpsmodule(){
	}

};


#endif
