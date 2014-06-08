#include "modules/arduino/arduinomodule.h"
#include "modules/arduino/arduinotags.h"
#include "modules/arduino/arduinoconfig.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include "event_flag.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include "logger.h"
	void* arduinomodule::thread(void* args){
		arduinomodule* module = (arduinomodule*) args;
		module->m_device = openSerialPort(module->path,115200,0,0);
		
			//block until data is read from arduino
			module->readArduino();
			//module->m_dataArrived=true;
		close(module->m_device);	
		return NULL;			
	}

	void arduinomodule::readArduino(){
		const int buffsize=4096;
		char buffer[buffsize];
		int size=0, incoming;
		char nlChar = '\n';
		while(running){
			while(m_cmds.size()>0){
				arduinocmd* my_cmd = m_cmds.back();
				m_cmds.pop_back();
				char outBuffer[1024];
				if(my_cmd->arduino_flag==FLAG_RESET_MOTORS){
					const char *msg = "M\n";
					write(m_device,msg,2);
				}
				
				delete my_cmd;
			}
			incoming = read(m_device,buffer+size,buffsize-size);
			if(incoming==-1){
				sleepms(10);
				continue;
			}
			size+=incoming;
			for(int i=0;i<size;i++){
				if(buffer[i] == nlChar ){
					//lets construct an event and push it out.
					arduinodata* data = getArduinoData();
					memcpy(data->data,buffer,i);
					Logger::log(m_moduleid,LOGGER_INFO,data->data);
					switch(data->data[0]){
						case ARDUINO_TAG_IMU:
							data->eflag = EFLAG_IMURAW;
						break;
						case ARDUINO_TAG_GPS:
							data->eflag = EFLAG_GPSRAW;
						break;
					}	
					data->numChar = i;
					m_events.push_back(data);
					memcpy(buffer,buffer+i+1,size-i-1);
					size-=i+1;
					i=-1;
				}
			}
		}
	}

	void arduinomodule::initializeReader(){
		spawnThread(arduinomodule::thread, this);
		m_dataArrived=false;
	}
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
	void arduinomodule::initialize(uint32& listener_flag){
	        listener_flag |= EFLAG_ARDUINOCMD;
		readPathConfig(cfgfile, m_moduleid, path);
		initializeReader();
	}

	void arduinomodule::printEvent(std::ostream& out, const event* evt){
		arduinodata* data = (arduinodata*)(evt->m_data);
		data->data[data->numChar]=0;
		out<< "arduino:"<<data->data<<std::endl;
	}
	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void arduinomodule::update(bot_info* data){
		while(!m_sent_events.empty()){
			m_recycler.push_back(m_sent_events.back());
			m_sent_events.pop_back();
		}
		while(!m_events.empty()){
			arduinodata* dataline = m_events.back();
			m_events.pop_back();
			event* evt = this->makeEvent(EFLAG_ARDUINORAW|dataline->eflag,dataline);
			evt->m_print= arduinomodule::printEvent;
			data->m_eventQueue.push_back(evt);
			m_sent_events.push_back(dataline);
		}
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/	
	void arduinomodule::pushEvent(event* evt){
		if(evt->m_eventflag == EFLAG_TERMINATE)
			running=false;	
		if(evt->m_eventflag == EFLAG_ARDUINOCMD){
			arduinocmd* cmd = (arduinocmd*)evt->m_data;
			m_cmds.push_back(cmd);	
		}	
	}

const char* arduinomodule::myName="Arduino Module";
