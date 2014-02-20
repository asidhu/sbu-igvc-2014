#ifndef _SAMPLE_MODULE_H
#define _SAMPLE_MODULE_H
#include "module.h"
#include <vector>
#include "basetypes.h"
class event;
class samplemodule:public module{
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
