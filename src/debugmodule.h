#ifndef _DEBUG_MODULE_H
#define _DEBUG_MODULE_H

#include "module.h"
#include <set>
#include <vector>
#include "basetypes.h"
#include <iostream>
#include <map>
#include "query.h"
#include "event_flag.h"

#define WARN_MODULE_TIMER	3000 
#define MAX_QUERY_TRACKER	1000		
//if module takes more than this amount of micro seconds print warning



class event;
class query_event_info{
public:
	bool acked;
	uint32 queryID;
	uint32 eventID;
	uint32 senderModuleId;
	uint32 timer;
	uint32 receiverModuleId;
};
class debugmodule:public module{
private:
	static const char*  myName;
	uint32 m_events_len;
	uint64 m_timer;
	query_event_info* m_qe_space[MAX_QUERY_TRACKER];
	std::multimap<uint32, query_event_info*> m_events;
	void addEvent(event*);
	void recycleInfo(query_event_info*);
	void processResponse(event*);
public:
	std::ostream *m_output;
	debugmodule(std::ostream* os){
		m_output=os;
		for(uint32 i=0;i<MAX_QUERY_TRACKER;i++)
			m_qe_space[i] = new query_event_info();
		m_events_len=0;
		m_timer=0;
	}

	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event*);
	const char* getCommonName(){
		return myName;
	};


};
#endif
