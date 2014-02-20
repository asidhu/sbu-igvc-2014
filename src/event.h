#ifndef _EVENT_H
#define _EVENT_H
#include "basetypes.h"
#include "oscompat.h"
#include "module.h"
class event{
	private:
	static uint64 m_event_counter;
	event(uint32 moduleid){
		m_eventid=m_event_counter++;
		m_eventflag=0;
		m_timestamp=getMicros();
		m_moduleid=moduleid;
		m_query.m_queryid=m_query.m_moduleid=0;
		m_query.m_querydata=(void*)0;
	}
	public:
	uint64 m_timestamp;
	uint32 m_eventid;
	uint32 m_eventflag;
	uint32 m_moduleid;
	union{
		struct{
			uint32	m_queryid;
			uint32	m_moduleid;
			void* 	m_querydata;
		}m_query;
		struct{
			uint32 	m_responseid;
			uint32 	m_moduleid;
			uint32 	m_query_id;
			uint32	m_query_event_id;
			void*	m_responsedata;
		}m_response;
		struct{
			uint32 	m_unused;
			uint32 	m_unused2;
			void*	m_cmdData;

		}m_command;
		struct{
			uint32 	m_unused;
			uint32	m_unused2;
			void*	m_data;
		}m_generic;
	};
	friend class module;
};

#endif
