#ifndef _IMU_MODULE_H
#define _IMU_MODULE_H
#include "module.h"
#include "modules/imu/imudata.h"
#include <iostream>
#include "basetypes.h"


#define IMU_REPORT_INTERVAL   		10000//in us
#define ARDUINO_PROTOCOL_HEADING	'H'
#define ARDUINO_PROTOCOL_ACCEL	 	'A'
#define ARDUINO_PROTOCOL_GYRO 		'G'

class imumodule:public module{
private:
	static const char* myName;
	static void printEvent(std::ostream&, const event*);
	imudata m_imudata;
	uint64 m_last_report_time;
	bool m_dataArrived;
public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	imumodule(){
		m_last_report_time=0;
		m_dataArrived=false;
	}

};


#endif
