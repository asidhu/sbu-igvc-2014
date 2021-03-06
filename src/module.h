#ifndef _MODULES_H
#define _MODULES_H
#include "basetypes.h"
#include "event_flag.h"
#include <vector>
class event;
class bot_info;
class module{
private:
	std::vector<event*> m_myEvents;
	event* createEvent();
protected:
	uint32 m_moduleid;
	void processEvent(event* e);
	event* makeEvent(uint32 evtFlag, void* data);

public:
	module():m_myEvents(){}
	void setID(uint32 id){	m_moduleid=id;	}
	uint32 getID(){	return m_moduleid;	}
	virtual void initialize(uint32&)=0;
	virtual void update(bot_info*)=0;
	virtual void pushEvent(event*)=0;
	virtual const char* getCommonName()=0;
	virtual void handleCommand(uint32 cmdflag, void* cmddata){}
	virtual void handleEvent(uint32 evtflag, uint32 moduleid, void* evtdata){}
	void recycleEvents();
};


#endif
