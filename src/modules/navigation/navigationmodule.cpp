#include "modules/navigation/navigationmodule.h"
#include "event.h"
#include "base.h"
#include "event_flag.h"
#include "osutils.h"
#include "modules/joystick/joystickmodule.h"
#include "modules/motors/motorctrl.h"
#include "modules/gps/gpsdata.h"
#include "modules/gps/gpsmodule.h"
#include "modules/imu/imudata.h"
#include "modules/arduino/arduinodata.h"	
#include "modules/navigation/waypointdata.h"
#include <stdlib.h>
#include <cmath>
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
		currentWaypoint->lat=42.67816288333;
		//currentWaypoint->lon=
		currentWaypoint->nextWaypoint=NULL;
		spawnThread(navigationmodule::thread,this);
	}


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void navigationmodule::update(bot_info* data){
		if(initializeMotors){
			arduinocmd* cmd = (arduinocmd*)malloc(sizeof(arduinocmd));
			cmd->arduino_flag = FLAG_RESET_MOTORS;
			event* evt = makeEvent(EFLAG_ARDUINOCMD,cmd);
			data->m_eventQueue.push_back(evt);	
			initializeMotors=false;
		}	
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
                 	processIMUEvent((imudata*)evt->m_data);
				
			break;
			case EFLAG_GPSDATA:
			processGPSEvent((gpsdata*)evt->m_data);		
			break;
		}	
	}
	
	void navigationmodule::processJSEvent(joystickevent* evt){
		switch(evt->type){
			case BUTTON_SAFETY:
				if(m_motors->offline){
					initializeMotors=true;	
				}
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

	void navigationmodule::processIMUEvent(imudata* evt){
         float tempHeading=evt->heading;
         tempHeading+=7.52;
         if ((tempHeading-360)>=0)
         {tempHeading-=360;}
         currentHeading=tempHeading;
         modified=true;
	}
	
	void* navigationmodule::thread(void* args){
		navigationmodule* module = (navigationmodule*)args;
		module->running=true;
		module->navigate();
		return NULL;
	}
	
	void navigationmodule::navigate()
	{
         while(running)
         {
               if(modified)
               {
                 modified=false;
                 float angleToWaypoint=angle(currentLat,currentLon,currentWaypoint->lat,currentWaypoint->lon);
                 angleToWaypoint*=-1;
                 angleToWaypoint+=90;
                 float difference=angleToWaypoint-currentHeading;
                 if (difference>2)
                 {
                    if (difference<180)
                    {
                       m_motors->left_power = -1;
					   m_motors->right_power = 1;
                    }
                    else 
                    {
                       m_motors->left_power = 1;
					   m_motors->right_power = -1;
                    }
                  }  
                 else if (difference<2)
                 {
                    if (difference>-180)
                    {
                       m_motors->left_power = 1;
					   m_motors->right_power = -1;
                    }
                    else 
                    {
                       m_motors->left_power = -1;
					   m_motors->right_power = 1;
                    }
                  } 
                  difference=((float)abs((double)difference));
                  if (difference>180)
                  {difference=360-difference;}
                  m_motors->throttle = ((difference/180)*0.5);
               }  
    	   }
   	}
	
	void navigationmodule::processGPSEvent(gpsdata* evt){
         float latitude = evt->gps[0].latitude;
         float longitude = evt->gps[0].longitude;
	 currentLat=latitude;
         currentLon=longitude;
         modified=true;
	}
const char* navigationmodule::myName="Navigation Module";
