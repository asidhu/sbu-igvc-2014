#include "module.h"
#include "event.h"


event* module::createEvent(){
	event* evt = new event(m_moduleid);
	m_myEvents.push_back(evt);
	return evt;
}

void module::processEvent(event* e){
	if(e->m_eventflag == EFLAG_TERMINATE)
		handleCommand(e->m_eventflag,e->m_data);
	else
		handleEvent(e->m_eventflag,e->m_moduleid,e->m_data);
}

event* module::makeEvent(uint32 evtFlag, void* data){
	event* evt = createEvent();
	evt->m_eventflag= evtFlag;
	evt->m_data = data;
	return evt;
}


void module::recycleEvents(){
	for(std::vector<event*>::iterator it = m_myEvents.begin();it!=m_myEvents.end();it++){
		delete *it;
	}
	m_myEvents.clear();
}
