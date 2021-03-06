#include "base.h"
#include "event.h"
#include "logger.h"
#include "samplemodule.h"//shouldnt be here
#include "modules/cpuinfo/cpuinfomodule.h"
#include "modules/network/networkmodule.h"
#include "modules/imu/imumodule.h"
#include "modules/joystick/joystickmodule.h"
#include "modules/motors/motormodule.h"
	#include "modules/camera/cameramodule.h"
#include "modules/navigation/navigationmodule.h"
#include "modules/gps/gpsmodule.h"
#include "modules/arduino/arduinomodule.h"
#include "osutils.h"
#include "event_flag.h"
#include "debugmodule.h"
#include "stdio.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
bot my_bot;
void terminate(int sig){
	my_bot.terminate();
}

int main(int argc, char** argv){
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler=terminate;
	sigaction(SIGTERM,&action,NULL);
	sigaction(SIGINT, &action,NULL);
	sigaction(SIGSTOP, &action, NULL);
	my_bot.initialize();
	my_bot.mainLoop();
}

void bot::initialize(){
	//load all modules.
	//m_modules.push_back(new samplemodule());
	m_modules.push_back(new cpuinfomodule());
	//m_modules.push_back(new networkmodule());
	m_modules.push_back(new joystickmodule());
	m_modules.push_back(new imumodule());
	m_modules.push_back(new gpsmodule());
	m_modules.push_back(new arduinomodule("cfg/arduino1.cfg"));
	m_modules.push_back(new arduinomodule("cfg/arduino2.cfg"));
	m_modules.push_back(new cameramodule());
	m_modules.push_back(new debugmodule(&std::cout));
	motormodule* mm = new motormodule();
	navigationmodule* nm = new navigationmodule(&mm->m_ctrl);
	m_modules.push_back(nm);
	m_modules.push_back(mm);
	Logger::initialize("cfg/logger.cfg",&m_modules);
	
	uint32 count=0;
	for(std::vector<module*>::iterator it = m_modules.begin(); it!=m_modules.end();it++){
		uint32 listener_flag =0;
		module* m = *it;
		m->setID(count);
		m->initialize(listener_flag);	
		if(listener_flag!=0)
			addListener(m,listener_flag);
		count++;
	}

	
}

void bot::addListener(module* m, uint32 flag){
	//check existing flags
	for(uint32 i=0;i<m_module_listeners_count;i++){
		if(m_module_listeners_flags[i]==flag){
			//found listener with same flags push and return.
			m_module_listeners[i].push_back(m);
			return;
		}	
	}
	//nothing found? oh well make a new one
	if(m_module_listeners_count>=MAX_LISTENER_LISTS)
		return;
	m_module_listeners[m_module_listeners_count].push_back(m);
	m_module_listeners_flags[m_module_listeners_count]=flag;
	m_module_listeners_count++;
}

void bot::dispatchEvent(event* evt){
	uint32 byteone = evt->m_eventflag & 0xF000000;
	if(byteone==EFLAG_TERMINATE){
		dispatchToList(m_modules,evt);
	}
	else{
		for(uint32 i=0;i< m_module_listeners_count;i++){
			if(evt->m_eventflag & m_module_listeners_flags[i]){
				dispatchToList(m_module_listeners[i],evt);
			}
		}
	}
}

void bot::dispatchToList(std::vector<module*>& list, event* evt){
	for(std::vector<module*>::iterator it = list.begin(); it!=list.end();it++){
		(*it)->pushEvent(evt);
	}
}

void bot::terminate(){
	terminateSelf=true;
	sleepms(1000);
	std::cout<<"Shutdown in 3..."<<std::endl;
	sleepms(1000);
	std::cout<<"Shutdown in 2..."<<std::endl;
	sleepms(1000);
	std::cout<<"Shutdown in 1..."<<std::endl;
	sleepms(1000);
	std::cout<<"Shutdown in 0..."<<std::endl;
	exit(0);
}

void bot::mainLoop(){
	uint32 benchmark[m_modules.size()];
	bot_info moduleInfo;
	moduleInfo.m_modules = &m_modules;
	moduleInfo.m_moduleTimer = (uint32*) benchmark;
	moduleInfo.m_moduleTimerLen = m_modules.size();
	while(1){
		moduleInfo.m_eventQueue.clear();
		if(terminateSignalSent)
			break;
		if(terminateSelf){
			event* evt = new event(0);
			evt->m_eventflag = EFLAG_TERMINATE;
			moduleInfo.m_eventQueue.push_back(evt);
			terminateSignalSent=true;		
		}
		uint32 counter=0;
		for(std::vector<module*>::iterator it = m_modules.begin(); it!=m_modules.end();it++){
			module* m = *it;
			uint64 duration = getMicros();
			moduleInfo.m_currentTime=duration;
			m->update(&moduleInfo);
			moduleInfo.m_moduleTimer[counter]=getMicros()-duration;
			counter++;
		}
		
		uint32 totalDuration=0;
		for(std::vector<event*>::iterator it = moduleInfo.m_eventQueue.begin(); it!=moduleInfo.m_eventQueue.end();){
			event* evt = *it;
			uint64 duration = getMicros();
			dispatchEvent(evt);
			totalDuration+= getMicros()-duration;
			it++;
			//this is okay because we clear the event queue afterwards anyway
		}
		for(std::vector<module*>::iterator it = m_modules.begin(); it!=m_modules.end();it++){
			module* m = *it;
			m->recycleEvents();
		}
		sleepms(SLEEP_TIME);
	}
}
