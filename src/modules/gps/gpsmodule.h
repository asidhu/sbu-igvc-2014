#ifndef _GPS_MODULE_H
#define _GPS_MODULE_H
#include "module.h"
#include "modules/gps/gpsdata.h"
#include <iostream>
#include <cmath>

#define GPS_REPORT_INTERVAL     10000 // in us
#define ARDUINO_PROTOCOL_TIME   'T'  
#define ARDUINO_PROTOCOL_DATE   'D'
#define ARDUINO_PROTOCOL_FIX    'F'
#define ARDUINO_PROTOCOL_LOC    'L'
#define ARDUINO_PROTOCOL_SPEED  'S'
#define ARDUINO_PROTOCOL_ANGLE  'A'
#define ARDUINO_PROTOCOL_ELEV   'E'
#define ARDUINO_PROTOCOL_SAT    'N'
#define EARTH_SEMIMAJOR         6378100 // in km
#define EARTH_SEMIMINOR         6356800 // in km
#define EARTH_RADIUS            6371000 // in km

class gpsmodule:public module{
private:
	static const char* myName;
	static void printEvent(std::ostream&, const event*);
	gpsdata m_gpsdata;
	uint64 m_last_report_time;
	bool m_dataArrived;
public:
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	gpsmodule(){
	  m_last_report_time=0;
	  m_dataArrived=false;
	}

};

/* Returns distance b/n two gps locations in meters */
float distance(float lat1, float long1, float lat2, float long2);

/* Returns the angle between North and your bearing */
float angle(float lat1, float long1, float lat2, float long2);

float toRadians(float degrees);
float toDegrees(float radians);
float normalRadius(float phi);
float toX(float lat, float lon);
float toY(float lat, float lon);

#endif
