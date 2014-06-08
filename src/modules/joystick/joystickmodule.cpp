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
#include <unistd.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <cstdlib>
#include <cstdio>
	void* joystickmodule::thread(void* args){
		joystickmodule* module = (joystickmodule*)args;
		module->running=true;
		module->calibration =0;
		module->loadConfig();
		strcpy(module->m_cfg.dev_name,"/dev/input/js0");
		module->pollEvents();
		return NULL;
	}

	void joystickmodule::sendBtnEvt(int TYPE, int value){
		joystickevent* evt = getEvent();
		evt->type=TYPE;
		evt->btnValue=value;
		m_event_queue.push_back(evt);	
	}
	
	void joystickmodule::sendAxisEvt(int TYPE, double value){
		joystickevent* evt = getEvent();
		evt->type=TYPE;
		evt->axisValue=value;
		m_event_queue.push_back(evt);	
	}
	void joystickmodule::pollEvents(){
		int fd_dev = open(m_cfg.dev_name,O_RDONLY);
		if(fd_dev==-1){
			Logger::log(m_moduleid,LOGGER_ERROR,"could not open the joystick device in config!");	
			return;
		}
		char controller_name[80];
		ioctl(fd_dev, JSIOCGNAME(80),&controller_name);
		Logger::log(m_moduleid,LOGGER_INFO,"Found controller %s",controller_name);
		//get information about controller.
		while(running){
			if (calibration==0)
			{
				calibrateJoyStick(fd_dev);
			}
			//get events
			struct js_event e;
			read(fd_dev, &e,sizeof(e));
			if(e.type== JS_EVENT_BUTTON){
				if(e.number == m_cfg.safety)
					sendBtnEvt(BUTTON_SAFETY,e.value);
				if(e.number == m_cfg.forward)
					sendBtnEvt(BUTTON_FORWARD,e.value);
				if(e.number == m_cfg.rotate_left)
					sendBtnEvt(BUTTON_ROTATE_LEFT,e.value);
				if(e.number == m_cfg.rotate_right)
					sendBtnEvt(BUTTON_ROTATE_RIGHT,e.value);
				if(e.number == m_cfg.backwards)
					sendBtnEvt(BUTTON_BACKWARDS,e.value);
			}
			if(e.type == JS_EVENT_AXIS){
				if(e.number == m_cfg.lt.id){
					double power = ((double)e.value/(m_cfg.lt.max-m_cfg.lt.min))+.5;
						sendAxisEvt(AXIS_THROTTLE,power);
				}
				
			} 
		}
		close(fd_dev);
			
	
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
		//return events in the sent queue to pool.
		m_event_pool.insert(m_event_pool.end(),m_event_sent.begin(),m_event_sent.end());
		m_event_sent.clear();
		
		
		//send events waiting in the queue
		std::vector<joystickevent*>::iterator it = m_event_queue.begin(),
			last = m_event_queue.end(),start;
		start=it;
		for(;it!=last;it++){
			joystickevent* js = *it;
			event* evt = makeEvent(EFLAG_JOYSTICKEVT,js);
			evt->m_print = joystickmodule::printevent;
			data->m_eventQueue.push_back(evt);
		}
		m_event_sent.insert(m_event_sent.end(),start,last);
		m_event_queue.erase(start,last);
	}

	void joystickmodule::printevent(std::ostream& out, const event* evt){
		joystickevent* js = (joystickevent*)evt->m_data;
		out <<"{ type:"<<js->type<<", value:";
		if(js->type== AXIS_THROTTLE)
			out << js->axisValue;
		else
			out << js->btnValue;
		out<<"}";
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void joystickmodule::pushEvent(event* evt){
		if(evt->m_eventflag==EFLAG_TERMINATE)
			running=false;	
	}

	/**
	 * Calibrates the configuration file to work with the controller
	 */ 
	void joystickmodule::calibrateJoyStick(int fd_dev)
	{
		short value;	// Value is 16 bits signed
		char number;	// Number is 8 bits unsigned
		while (calibration<7 && running)
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
					if(read(fd_dev,&e,sizeof(e))==0)continue;
					number=e.number;
					value=e.value;
				}while(value!=1 && e.type!=JS_EVENT_BUTTON && running);
				switch(calibration){
					case 0: m_cfg.safety=number;break;
					case 1: if(number!=m_cfg.safety)m_cfg.forward=number;
						else continue;
						break;
					case 2: if(number!=m_cfg.safety && number!=m_cfg.forward)
						m_cfg.rotate_left=number;
						else continue;
						break;
					case 3: if(number!=m_cfg.safety && number!=m_cfg.forward
							&& number!=m_cfg.rotate_left)
							m_cfg.rotate_right=number;
						else continue;
						break;
					case 4: if(number!=m_cfg.safety && number!=m_cfg.forward
							&& number!=m_cfg.rotate_left && number!= m_cfg.rotate_right)
								m_cfg.backwards=number;
						else continue;
						break;

				}
			}
			else{
				struct js_event e;
				char axis;
				short min,max;
				bool axis_picked=false;
				do{
					if(read(fd_dev, &e,sizeof(e))==0)continue;
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
							Logger::log(m_moduleid,LOGGER_INFO,"Picked axis %d",number);
							axis = number;
							min=max=value;
						}
					}else{
						if(e.type==JS_EVENT_BUTTON){
							if(number==m_cfg.safety){
								Logger::log(m_moduleid,LOGGER_INFO,"Saved axis min:%d max:%d",min,max);
								break;
							}
							else
								Logger::log(m_moduleid,LOGGER_WARNING,"Thats not the safety button!!");
						}else{
							if(number==axis){
								if(value>max)max=value;
								if(value<min)min=value;
							}
							
						}
						
					}
					
				}while(running);
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
		saveConfig();
	}


void joystickmodule::loadConfig(){
	using namespace rapidxml;
	char buff[4096];
	long sz=4096;
	readFile("cfg/joystick.cfg",buff,sz);
	buff[sz]=0;
	if(sz<=0){
		Logger::log(m_moduleid,LOGGER_WARNING,"Joystick configuration not found...");
		return;
	}
	xml_document<> doc;
	doc.parse<0>(buff);
	xml_node<> *body = doc.first_node();
	xml_node<> *forward = body->first_node();
	xml_node<> *backward = forward->next_sibling();
	xml_node<> *safety = backward->next_sibling();
	xml_node<> *rlctrl = safety->next_sibling();
	xml_node<> *rrctrl =rlctrl->next_sibling();
	xml_node<> *throttle =rrctrl->next_sibling();
	xml_node<> *throttlemin =throttle->next_sibling();
	xml_node<> *throttlemax =throttlemin->next_sibling();
	m_cfg.forward = (char)atoi(forward->value());	
	m_cfg.backwards = (char)atoi(backward->value());	
	m_cfg.rotate_left = (char)atoi(rlctrl->value());	
	m_cfg.rotate_right = (char)atoi(rrctrl->value());	
	m_cfg.safety = (char)atoi(safety->value());	
	m_cfg.lt.id = (char)atoi(throttle->value());	
	m_cfg.lt.min = atoi(throttlemin->value());	
	m_cfg.lt.max = atoi(throttlemax->value());	
	Logger::log(m_moduleid,LOGGER_INFO,"Joystick configuration loaded.");
	calibration=8;
}

void joystickmodule::saveConfig(){
	using namespace rapidxml;
	xml_document<> doc;
	const char* name = "joystick",
		*fctrl = "forward",
		*bctrl = "backward",
		*rrctrl= "rotateright",
		*rlctrl= "rotateleft",
		*safety = "safety",
		*throttle = "throttle",
		*throttlemin = "throttlemin",
		*throttlemax = "throttlemax";
	xml_node<> *body = doc.allocate_node(node_element,name);
	doc.append_node(body);
	char buff[64];
	sprintf(buff,"%d",m_cfg.forward);
	body->append_node( doc.allocate_node(node_element,fctrl,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.backwards);
	body->append_node( doc.allocate_node(node_element,bctrl,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.safety);
	body->append_node( doc.allocate_node(node_element,safety,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.rotate_left);
	body->append_node( doc.allocate_node(node_element,rlctrl,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.rotate_right);
	body->append_node( doc.allocate_node(node_element,rrctrl,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.lt.id);
	body->append_node( doc.allocate_node(node_element,throttle,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.lt.min);
	body->append_node( doc.allocate_node(node_element,throttlemin,doc.allocate_string(buff)));
	sprintf(buff,"%d",m_cfg.lt.max);
	body->append_node( doc.allocate_node(node_element,throttlemax,doc.allocate_string(buff)));

	char writebuff[2048];
	char* end = print(writebuff,doc,0);
	*end=0;
	writeFile("cfg/joystick.cfg",writebuff,strlen(writebuff));
	Logger::log(m_moduleid,LOGGER_INFO,"Joystick configuration saved.");
		
}

const char* joystickmodule::myName="Joystick Module";
