#include "event.h"

uint64 event::m_event_counter=0;


std::ostream& operator<<(std::ostream& out, const event* mc){
	if(mc->m_print!=NULL){
		mc->m_print(out,mc);
	}
	else{
		out<< "event(module:"<< mc->m_moduleid<<",id:"<<mc->m_eventid<<", flag:"<< mc->m_eventflag << "):unknown data"<<std::endl;
	}
	return out;
}
