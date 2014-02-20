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
	event* sendQuery(uint32 target, uint32 queryid, void* data);
	event* sendEvent(uint32 evtFlag, void* data);
	event* sendResponse(event* query, uint32 responseid, void* data);

public:
	void setID(uint32 id){	m_moduleid=id;	}
	uint32 getID(){	return m_moduleid;	}
	virtual void initialize(uint32&)=0;
	virtual void update(bot_info*)=0;
	virtual void pushEvent(event*)=0;
	virtual const char* getCommonName()=0;
	virtual void handleCommand(uint32 cmdflag, void* cmddata){}
	virtual void handleQuery(uint32 queryid, void* querydata, event* e){}
	virtual void handleResponse(uint32 queryid, uint32 responseid, void* respdata){}
	virtual void handleEvent(uint32 evtflag, uint32 moduleid, void* evtdata){}
	void recycleEvents();
};


#endif
