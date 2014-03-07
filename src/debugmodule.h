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

class debugmodule:public module{
private:
	static const char*  myName;
	uint32 m_events_len;
	uint64 m_timer;
public:
	std::ostream *m_output;
	debugmodule(std::ostream* os){
		m_output=os;
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
