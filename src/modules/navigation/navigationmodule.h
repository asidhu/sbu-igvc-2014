#ifndef _NAVIGATION_MODULE_H
#define _NAVIGATION_MODULE_H
#include "module.h"

#define MODE_MANUAL	 	0x01
#define MODE_AUTONOMOUS 	0x02
#define MODE_STATIONARY		0x03
class motorctrl;
class joystickevent;
struct gpsdata;
struct imudata;
struct waypointdata;
class navigationmodule:public module{
	private:
	static const char* myName;
	motorctrl* m_motors;
	volatile bool initializeMotors;
	void processJSEvent(joystickevent*);
	void processGPSEvent(gpsdata*);
	void processIMUEvent(imudata*);
	volatile float currentHeading;
	volatile float currentLat;
	volatile float currentLon;
    volatile bool modified;
	volatile bool running;
	waypointdata* currentWaypoint;
	
	public:
	int m_navmode;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	static void* thread(void* args);
	navigationmodule(motorctrl* mc){
		m_motors=mc;
	}
	const char* getCommonName(){
		return myName;
	}

};


#endif
