#include "modules/joystick/joystickmodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include "logger.h"
#include <string.h>
	void* joystickmodule::thread(void* args){
		joystickmodule* module = (joystickmodule*)args;
		module->polling=true;
		module->calibration =0;
		strcpy(module->m_cfg.dev_name,"/dev/input/js0");
		module->pollEvents();	
		return NULL;
	}

	

	void joystickmodule::pollEvents(){
		int fd_dev = open(m_cfg.dev_name,O_RDONLY);
		if(fd_dev==-1){
			Logger::log(m_moduleid,LOGGER_ERROR,"could not open the joystick device in config!");	
			return;
		}
		//get information about controller.
		if (calibration==0)
		{
			calibrateJoyStick(fd_dev);
		}
		while(polling){
			struct js_event e;
			read(fd_dev, &e,sizeof(e));
			
		}
			
	
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
	void joystickmodule::initialize(uint32& listener_flag){
		spawnThread(joystickmodule::thread,this);
	}


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void joystickmodule::update(bot_info* data){
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void joystickmodule::pushEvent(event* evt){
		
	}
	/**
	 * Calibrates the configuration file to work with the controller
	 */ 
	void joystickmodule::calibrateJoyStick(int fd_dev)
	{
		short value;	// Value is 16 bits signed
		char number;	// Number is 8 bits unsigned
		while (calibration<7)
		{	
			switch(calibration)
			{
				
				case 0: Logger::log(m_moduleid,LOGGER_INFO,"Press the button you would like to use for safety...");
					break;
				case 1: Logger::log(m_moduleid,LOGGER_INFO,"Press the button you would like to use for forward...");
					break;
				case 2: Logger::log(m_moduleid,LOGGER_INFO,"Press the button you would like to use to rotate left...");
					break;
				case 3: Logger::log(m_moduleid,LOGGER_INFO,"Press the button you would like to use to rotate right...");
					break;
				case 4: Logger::log(m_moduleid,LOGGER_INFO,"Press the button you would like to use for backwards...");
					break;
				case 5: Logger::log(m_moduleid,LOGGER_INFO,"Click left trigger completely and then release it completely...");
					Logger::log(m_moduleid,LOGGER_INFO,"Then hit the button you chose for safety...");
					break;
				case 6: Logger::log(m_moduleid,LOGGER_INFO,"Click right trigger completely and then release it completely...");
					Logger::log(m_moduleid,LOGGER_INFO,"Then hit the button you chose for safety...");
					break;
				default: Logger::log(m_moduleid,LOGGER_ERROR,"calibration error");
			}
			if(calibration<5){
				struct js_event e;
				do{
					read(fd_dev,&e,sizeof(e));
					number=e.number;
					value=e.value;
				}while(value!=1 && e.type!=JS_EVENT_BUTTON);
				switch(calibration){
					case 0: m_cfg.safety=number;break;
					case 1: if(number!=m_cfg.safety)m_cfg.forward=number;break;
					case 2: if(number!=m_cfg.safety || number!=m_cfg.forward)m_cfg.rotate_left=number;break;
					case 3: if(number!=m_cfg.safety || number!=m_cfg.forward
							|| number!=m_cfg.rotate_left)m_cfg.rotate_right=number;break;
					case 4: if(number!=m_cfg.safety || number!=m_cfg.forward
							|| number!=m_cfg.rotate_left || number!= m_cfg.rotate_right)
								m_cfg.backwards=number;break;

				}
			}
			else{
				struct js_event e;
				char axis;
				short min,max;
				bool axis_picked=false;
				do{
					read(fd_dev, &e,sizeof(e));
					number=e.number;
					value=e.value;
					if(e.type & JS_EVENT_INIT)continue;
					e.type&=~JS_EVENT_INIT;
					if(!axis_picked){
						if(e.type==JS_EVENT_BUTTON){
							Logger::log(m_moduleid,LOGGER_WARNING,"don't hit a button, hit the left or right trigger.");
							value = 0XFF;
							continue;
						}
						else if(calibration==6 && number == m_cfg.lt.id){
							Logger::log(m_moduleid,LOGGER_WARNING,"you picked the same axis!!! WHY??");
						}
						else{
							axis_picked=true;
							axis = number;
							min=max=value;
						}
					}else{
						if(e.type==JS_EVENT_BUTTON){
							if(number==m_cfg.safety)
								break;
							else
								Logger::log(m_moduleid,LOGGER_WARNING,"Thats not the safety button!!");
						}else{
							if(number==axis){
								if(value>max)max=value;
								if(value<min)min=value;
							}
							
						}
						
					}
					
				}while(true);
				if(calibration==5)
				{
					m_cfg.lt.id=axis;
					m_cfg.lt.min=min;
					m_cfg.lt.max=max;
				}
				else if(calibration==6)
				{
					m_cfg.rt.id=axis;
					m_cfg.rt.min=min;
					m_cfg.rt.max=max;
				}
			}
			++calibration;
		}
	}


const char* joystickmodule::myName="Joystick Module";
