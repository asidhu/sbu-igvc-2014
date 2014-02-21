#ifndef _CPUINFO_MODULE_H
#define _CPUINFO_MODULE_H
#include "module.h"
class cpuinfomodule:public module{
	private:
	static const char* myName;
	public:
	
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}

};


#endif
