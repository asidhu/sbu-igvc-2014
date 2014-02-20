#include "base.h"
#include "event.h"
#include "samplemodule.h"
#include "event_flag.h"
#include "debugmodule.h"
#include "stdio.h"
#include <iostream>
int main(int argc, char** argv){
	bot b;
	b.initialize();
	b.mainLoop();
}

void bot::initialize(){
	//load all modules.
	m_modules.push_back(new samplemodule());	
	m_modules.push_back(new debugmodule(&std::cout));	
	
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
	if(byteone==TERMINATE_EVENT_FLAG){
		dispatchToList(m_modules,evt);
	}
	else if(byteone==QUERY_EVENT_FLAG|| byteone == RESPONSE_EVENT_FLAG){
		uint32 target = (byteone==QUERY_EVENT_FLAG)?evt->m_query.m_moduleid:evt->m_response.m_moduleid;
		if(target<m_modules.size())
			m_modules[target]->pushEvent(evt);
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

void bot::mainLoop(){
	uint32 benchmark[m_modules.size()];
	bot_info moduleInfo;
	moduleInfo.m_modules = &m_modules;
	moduleInfo.m_moduleTimer = (uint32*) benchmark;
	moduleInfo.m_moduleTimerLen = m_modules.size();
	while(1){
		moduleInfo.m_eventQueue.clear();
		uint32 counter=0;
		for(std::vector<module*>::iterator it = m_modules.begin(); it!=m_modules.end();it++){
			module* m = *it;
			uint64 duration = getMicros();
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
	}
}