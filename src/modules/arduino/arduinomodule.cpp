#include "modules/arduino/arduinomodule.h"
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
#include <string.h>

	void* arduinomodule::thread(void* args){
		arduinomodule* module = (arduinomodule*) args;
		while(true){
			//block until data is read from arduino
			module->m_dataArrived=true;
		}
		return NULL;			
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
		initializeReader();
	}

	void arduinomodule::printEvent(std::ostream& out, const event* evt){
		arduinodata* data = (arduinodata*)(evt->m_data);
		out<< "arduino:"<<std::endl;
	}
	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void arduinomodule::update(bot_info* data){
		if(m_dataArrived){
			event* evt = this->makeEvent(m_data.eflag,&m_data);
			evt->m_print= arduinomodule::printEvent;
			data->m_eventQueue.push_back(evt);
			m_dataArrived=false;
		}
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/	
	void arduinomodule::pushEvent(event* evt){
		
	}

const char* arduinomodule::myName="Arduino Module";
