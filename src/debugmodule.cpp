#include "debugmodule.h"
#include "event.h"
#include "base.h"
void debugmodule::initialize(uint32& flag){
	flag=0;
}

void debugmodule::update(bot_info* data){
	//this only works if debug module is at the end.. geez.


	//lets check the times each module took to complete and report any offenders.	
	for(uint32 i=0;i<data->m_moduleTimerLen;i++){
		if(data->m_moduleTimer[i]>WARN_MODULE_TIMER){
			*m_output<< myName <<": module has exceeded the recommended execution time. time:"<<data->m_moduleTimer[i]
				<< " us, module: " << data->m_modules->at(i)->getCommonName()<<"(id="<<i<<")"<<std::endl;
		}
		//*m_output<< myName << ": module " << data->m_modules->at(i)->getCommonName() << " took " << data->m_moduleTimer[i] << 
		//	" us."<<std::endl;
	}
	//process events in the event queue.
	std::vector<event*> evtQueue = data->m_eventQueue;
	for(std::vector<event*>::iterator it = evtQueue.begin();it != evtQueue.end();it++){
		event* evt = *it;
		if(evt->m_eventflag==RESPONSE_EVENT_FLAG &&
			(evt->m_response.m_responseflag==RESPONSE_ACK || 
			evt->m_response.m_responseflag ==RESPONSE_BUSY ||
			evt->m_response.m_responseflag ==RESPONSE_SUCCESS ||
			evt->m_response.m_responseflag ==RESPONSE_FAILURE)){
			processResponse(evt);
		}
	}
	//check old events for failures.
	for(std::multimap<uint32,query_event_info*>::iterator it = m_events.begin();it!=m_events.end();it++){
		query_event_info* nfo = it->second;
		if(!nfo->acked){
			*m_output<< myName <<": module("<< nfo->receiverModuleId <<") did not ACK or respond to query."<<std::endl;
		}
	}
	
	//lets check our event queue for any new queries
	for(std::vector<event*>::iterator it = evtQueue.begin();it != evtQueue.end();it++){
		event* evt = *it;
		if(evt->m_eventflag==QUERY_EVENT_FLAG){
			addEvent(evt);
		}
	}
}

void debugmodule::processResponse(event* evt){
	std::pair<std::multimap<uint32,query_event_info*>::iterator,
		std::multimap<uint32,query_event_info*>::iterator> pair = m_events.equal_range(evt->m_response.m_moduleid);

	if(pair.first==pair.second){
		*m_output << myName << ": module("<< evt->m_response.m_moduleid<<") seems to be responding to unknown queries or the queryeventid field is not set correctly."<<std::endl;
		return;
	}
	
	for(std::multimap<uint32,query_event_info*>::iterator it = pair.first; it!=pair.second;it++){
		query_event_info* nfo = it->second;
		if(nfo->eventID == evt->m_response.m_query_eventid){
			//response to our event yay
			if(evt->m_response.m_responseflag== RESPONSE_ACK){
				nfo->acked=true;
			}
			else{
				m_events.erase(it);
				recycleInfo(nfo);
				return;
			}
		}
	}
	*m_output << myName << ": module("<< evt->m_response.m_moduleid<<") seems to be responding to unknown queries or the queryeventid field is not set correctly."<<std::endl;

}

void debugmodule::recycleInfo(query_event_info* nfo){
	if(m_events_len<=1){
		*m_output << myName << ": wtf happened? recycling twice?"<< std::endl;
		return;
	} 
	m_qe_space[m_events_len-1]=nfo;
	m_events_len--;
}

void debugmodule::addEvent(event* evt){
	//if m_events_len is greater than max number lets not insert it.
	if(m_events_len>=MAX_QUERY_TRACKER){
		*m_output << myName << ": seems like some modules are taking too long to respond. Query has been dropped."<<std::endl;
		return;
	}	
	m_qe_space[m_events_len]->queryID=evt->m_query.m_queryflag;
	m_qe_space[m_events_len]->eventID=evt->m_eventid;
	m_qe_space[m_events_len]->timer=m_timer;
	m_qe_space[m_events_len]->senderModuleId=evt->m_moduleid;
	m_qe_space[m_events_len]->receiverModuleId=evt->m_query.m_moduleid;
	m_qe_space[m_events_len]->acked=false;
	m_events.insert(std::pair<uint32,query_event_info*>(evt->m_moduleid,m_qe_space[m_events_len]));
	m_events_len++;
}

void debugmodule::pushEvent(event* evt){

}

const char* debugmodule::myName="Debug Module";
