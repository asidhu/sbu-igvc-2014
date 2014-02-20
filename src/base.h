#ifndef _BASE
#define _BASE
#include <vector>
#include "module.h"
#include "basetypes.h"
#define MAX_LISTENER_LISTS 512
class bot{
	private:
	std::vector<module*> m_modules;
	std::vector<module*> m_module_listeners[MAX_LISTENER_LISTS];
	uint32 m_module_listeners_flags[MAX_LISTENER_LISTS];
	uint32 m_module_listeners_count;
	void addListener(module*,uint32);
	void dispatchEvent(event*);
	void dispatchToList(std::vector<module*>&, event*);
	public:
	void initialize();
	void mainLoop();
	bot(){
		m_modules.clear();
		for(int i=0;i<MAX_LISTENER_LISTS;i++)
			m_module_listeners[i].clear();
		m_module_listeners_count=0;	
	}	
	
};

struct bot_info{
	std::vector<event*> m_eventQueue;
	std::vector<module*>* m_modules;
	uint32* m_moduleTimer;
	uint32 m_moduleTimerLen;
};

#endif

