#include "modules/navigation/navigationmodule.h"
#include "event.h"
#include "base.h"
#include "event_flag.h"
#include "modules/joystick/joystickmodule.h"
#include "modules/motors/motorctrl.h"
#include "modules/gps/gpsdata.h"
#include "modules/imu/imudata.h"
	/**
		rules of module ettiquette:
		1) only create a thread if you are reading from a file, doing io actions, socket ops, sleeping, blocking OR intensive calculations being performed. (AKA camera manipulation operations, intense geometric operations, anything greater than 5 ms worth of time)
		2) all threads should be monitored and kept track of and cleaned up when asked to.
		3) all modules should respond to standard commands.
	**/
	
	/**
		Rules:
		1) FEEL FREE to read information in from files. 
		2) If this is like the camera, and a command comes in to connect to camera, dont read configuration than. Load configuration NOW.
		3) dont open hardware or pipes or sockets now, there is a command for that. Open it at that time, only read files.
		4) listeners should also be setup HERE. If you want to listen to some event, take listener_flag and | it with the event flag.
	**/
	void navigationmodule::initialize(uint32& listener_flag){
		m_navmode = MODE_MANUAL;
		//Get joystick events for manual ctrl
		listener_flag|=	EFLAG_JOYSTICKEVT;
		//Get GPS and IMU events for localization and pose estimation
		listener_flag|= EFLAG_GPSDATA | EFLAG_IMUDATA;
	}


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void navigationmodule::update(bot_info* data){
	
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void navigationmodule::pushEvent(event* evt){
		switch(evt->m_eventflag){
			case EFLAG_JOYSTICKEVT:
				if(m_navmode == MODE_MANUAL){
					joystickevent* event = (joystickevent*)evt->m_data;
					processJSEvent(event);
						
				}
			break;
			case EFLAG_IMUDATA:
				
			break;
			case EFLAG_GPSDATA:
				
			break;
		}	
	}
	
	void navigationmodule::processJSEvent(joystickevent* evt){
		switch(evt->type){
			case BUTTON_SAFETY:
				m_motors->safety = evt->btnValue==0;
				m_motors->left_power = m_motors->right_power = m_motors->throttle =0;
			break;
			case BUTTON_FORWARD:
				if(evt->btnValue)
					m_motors->left_power = m_motors->right_power =1;
				else
					m_motors->left_power= m_motors->right_power =0;
			break;
			case BUTTON_BACKWARDS:
				if(evt->btnValue)
					m_motors->left_power = m_motors->right_power = -1;
				else
					m_motors->left_power= m_motors->right_power =0;
			break;
			case BUTTON_ROTATE_LEFT:
				if(evt->btnValue){
					m_motors->left_power = 1;
					m_motors->right_power = -1;
				}
				else
					m_motors->left_power= m_motors->right_power =0;
			break;
			case BUTTON_ROTATE_RIGHT:
				if(evt->btnValue){
					m_motors->left_power = -1;
					m_motors->right_power = 1;
				}
				else
					m_motors->left_power= m_motors->right_power =0;
			break;
			case AXIS_THROTTLE:
				m_motors->throttle = evt->axisValue;
			break;
		}
	
	}

	void navigationmodule::processIMUEvent(imudata*){

	}
	
	void navigationmodule::processGPSEvent(gpsdata*){

	}
const char* navigationmodule::myName="Navigation Module";
