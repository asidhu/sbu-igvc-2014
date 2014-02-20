#include "module.h"
#include "event.h"


event* module::createEvent(){
	event* evt = new event(m_moduleid);
	m_myEvents.push_back(evt);
	return evt;
}

void module::processEvent(event* e){
	if(e->m_eventflag == QUERY_EVENT_FLAG)
		handleQuery(e->m_query.m_queryflag, e->m_query.m_querydata,e);
	else if(e->m_eventflag == RESPONSE_EVENT_FLAG)
		handleResponse(e->m_response.m_queryflag,e->m_response.m_responseflag,e->m_response.m_responsedata);
	else if(e->m_eventflag == TERMINATE_EVENT_FLAG)
		handleCommand(e->m_eventflag,e->m_command.m_cmdData);
	else
		handleEvent(e->m_eventflag,e->m_moduleid,e->m_generic.m_data);
}

event* module::sendQuery(uint32 target, uint32 queryid, void* data){
	event* evt = createEvent();
	evt->m_eventflag= QUERY_EVENT_FLAG;
	evt->m_query.m_queryflag=queryid;
	evt->m_query.m_moduleid=target;
	evt->m_query.m_querydata=data;
	return evt;
}
event* module::sendEvent(uint32 evtFlag, void* data){
	event* evt = createEvent();
	evt->m_eventflag= evtFlag;
	evt->m_generic.m_data = data;
	return evt;
}


void module::recycleEvents(){
	for(std::vector<event*>::iterator it = m_myEvents.begin();it!=m_myEvents.end();it++){
		delete *it;
	}
	m_myEvents.clear();
}

event* module::sendResponse(event* query, uint32 responseid, void* data){
	event* evt = createEvent();
	evt->m_eventflag= RESPONSE_EVENT_FLAG;
	evt->m_response.m_moduleid= query->m_moduleid;
	evt->m_response.m_query_eventid = query->m_eventid;
	evt->m_response.m_queryflag = query->m_query.m_queryflag;
	evt->m_response.m_responseflag=responseid;
	evt->m_response.m_responsedata=data;
	return evt;
}
