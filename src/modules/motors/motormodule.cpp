#include "motormodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "logger.h"
#include <cstdio>
#include <cmath>
	void* motormodule::thread(void* args){
		motormodule* module = (motormodule*)args;
		module->running=true;
		module->manageMotors();
		return NULL;
	}

	void motormodule::manageMotors(){
		int dev_fd;
		dev_fd = openSerialPort(dev_name,9600,PARENB,0,CS7);	
		if(dev_fd == -1)
		{
			Logger::log(m_moduleid,LOGGER_ERROR,"Motors serial usb adapter not found!");
			return;
		}
		const char* INPUT_QUERY = "?E\r";
		const char* MOVE_QUERY = "!%c%02X\r";
		while(running){
			//first thing is check for battery voltage :D
			write(dev_fd,INPUT_QUERY, strlen(INPUT_QUERY));
			char buff[64];
			int left_power = (int)(std::abs(m_ctrl.left_power)*m_ctrl.throttle*0x7f);
			int right_power = (int)(std::abs(m_ctrl.right_power)*m_ctrl.throttle*0x7f);
			char LEFT = (m_ctrl.left_power >0)?'A':'a';
			char RIGHT = (m_ctrl.right_power > 0)?'B':'b';
			if(m_ctrl.safety){
				left_power=0;
				right_power=0;
			}
			sprintf(buff,MOVE_QUERY,LEFT,left_power);
			write(dev_fd,buff,5);
			sprintf(buff,MOVE_QUERY,RIGHT,right_power);
			write(dev_fd,buff,5);
			sleepms(50);
			//13 for echo, 6 for voltage, 4 for motors
			int n =read(dev_fd,buff,13+6+4);
			if(n==0){
				m_ctrl.offline=true;
			}
			else{
				m_ctrl.offline=false;
				buff[n]=0;
				int internal, battery;
				sscanf(buff,"?E\r%x\r%x",&internal, &battery);
				float iV = internal*28.5f/256;
				float bV = battery*55.f/256;
				Logger::log(m_moduleid,LOGGER_INFO,"Motor voltage:Int %f Bat:%f",iV,bV);	
			}
		}
		close(dev_fd);
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
	void motormodule::initialize(uint32& listener_flag){
		const char* name = "/dev/ttyUSB0";
		strcpy(dev_name,name);
		spawnThread(motormodule::thread,this);	
	}


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void motormodule::update(bot_info* data){
		
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void motormodule::pushEvent(event* evt){
		if(evt->m_eventflag== EFLAG_TERMINATE)
			running=false;
	}
const char* motormodule::myName="Motor Module";
