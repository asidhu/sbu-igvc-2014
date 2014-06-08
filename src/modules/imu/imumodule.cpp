#include "modules/imu/imumodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include "event_flag.h"
#include <iostream>
#include "modules/arduino/arduinodata.h"
#include "modules/arduino/arduinotags.h"
#include "logger.h"
#include <cstdio>
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
	void imumodule::initialize(uint32& listener_flag){
		listener_flag |= EFLAG_IMURAW;
	}

	void imumodule::printEvent(std::ostream& out, const event* evt){
		imudata* imu = (imudata*)(evt->m_data);
		out<< "imu:ACCEL"<< imu->A_X <<" " << imu->A_Y << " " << imu->A_Z;
		out<< "imu:HEADING"<< imu->heading;
		out<< "imu:GYRO"<< imu->G_Y <<" " << imu->G_P << " " << imu->G_R << std::endl;
	}
	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void imumodule::update(bot_info* data){
		uint64 timegap = std::abs(data->m_currentTime - m_last_report_time);
		//after receiving event, parse it, and push to world.
		if(m_dataArrived && timegap > IMU_REPORT_INTERVAL){
			event* evt = this->makeEvent(EFLAG_IMUDATA,&m_imudata);
			evt->m_print= imumodule::printEvent;
			data->m_eventQueue.push_back(evt);
			m_dataArrived=false;
			m_last_report_time = data->m_currentTime;
		}
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void imumodule::pushEvent(event* evt){
		const char* HEADING_FORMAT = "R,H,%f";
		const char* ACCEL_FORMAT = "R,A,%f,%f,%f";
		const char* GYRO_FORMAT = "R,G,%f,%f,%f";
		//read and parse event and create another event to push in next update.
		if(evt->m_eventflag & EFLAG_IMURAW && evt->m_eventflag & EFLAG_ARDUINORAW){
			//only parse if its an event from arduino, otherwise we might not understand it.
			arduinodata* data = (arduinodata*)(evt->m_data);
			if(data->data[0]==ARDUINO_TAG_IMU){
				//Make sure protocol is correct
				float params[3];
				int nParams;
				switch(data->data[2]){
					case ARDUINO_PROTOCOL_HEADING://Heading data arrived
						nParams=sscanf(data->data,HEADING_FORMAT,params);
						if(nParams==1){
							m_imudata.heading = params[0];
							//notify data has arrived
							m_dataArrived=true;
						}
						else
							Logger::log(m_moduleid,LOGGER_WARNING,"Failed to read the heading from arduino message!");
					break;
					case ARDUINO_PROTOCOL_ACCEL://Accelerometer data arrived
						nParams=sscanf(data->data,ACCEL_FORMAT,params,params+1,params+2);
						if(nParams==3){
							m_imudata.A_X = params[0];
							m_imudata.A_Y = params[1];
							m_imudata.A_Z = params[2];
							//notify data has arrived
							m_dataArrived=true;
						}
						else
							Logger::log(m_moduleid,LOGGER_WARNING,"Failed to read the accelerometer data from arduino message!");
						
					break;
					case ARDUINO_PROTOCOL_GYRO://Gyroscope data arrived
						nParams=sscanf(data->data,GYRO_FORMAT,params,params+1,params+2);
						if(nParams==3){
							m_imudata.G_Y = params[0];
							m_imudata.G_P = params[1];
							m_imudata.G_R = params[2];
							//notify data has arrived
							m_dataArrived=true;
						}
						else
							Logger::log(m_moduleid,LOGGER_WARNING,"Failed to read the gyroscope data from arduino message!");
						
					break;
					default:
					Logger::log(m_moduleid,LOGGER_WARNING,"Protocol Error! Control Character doesn't match expected chars.");
				}
			}
			else
				Logger::log(m_moduleid, LOGGER_WARNING, "Arduino module sent data with wrong tag?");	
		}
	}

const char* imumodule::myName="IMU Module";
