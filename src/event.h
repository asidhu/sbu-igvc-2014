#ifndef _EVENT_H
#define _EVENT_H
#include "basetypes.h"
#include "oscompat.h"
#include "module.h"
#include <iostream>
class event{
	private:
	static uint64 m_event_counter;
	event(uint32 moduleid){
		m_eventid=m_event_counter++;
		m_eventflag=0;
		m_timestamp=getMicros();
		m_moduleid=moduleid;
		m_data=(void*)0;
		m_print=NULL;
	}
	public:
	uint64 m_timestamp;
	uint32 m_eventid;
	uint32 m_eventflag;
	uint32 m_moduleid;
	void* m_data;
	void (*m_print)(std::ostream& out, const event* evt);
	friend class module;
};
std::ostream& operator<<(std::ostream& out, const event* mc);


#endif
