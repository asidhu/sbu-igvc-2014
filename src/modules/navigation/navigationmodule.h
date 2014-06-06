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

struct position {
  double x, y;
};

class navigationmodule:public module{
	private:
  struct gpsdata* m_gpsdata;
  struct imudata* m_imudata;
  double priorErrorCovar;                // for Kalman
  bool modifiedGPS;
  bool modifiedIMU;
	static const char* myName;
	motorctrl* m_motors;
	void processJSEvent(joystickevent*);
	void processGPSEvent(gpsdata*);
	void processIMUEvent(imudata*);
	static void* thread(void*);
	void navigate();
	void updatePos();
	void kalmanIteration(double 
	public:
	int m_navmode;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}

};


#endif
