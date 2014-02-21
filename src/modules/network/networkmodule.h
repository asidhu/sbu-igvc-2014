#ifndef _NETWORK_MODULE_H
#define _NETWORK_MODULE_H
#include "module.h"
class networkmodule:public module{
	private:
	static const char* myName;
	public:
	static void* thread(void* mod);
	void serverlisten();
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	void openSockFailure(const char*){}
};

struct cli_handlerinfo{
	networkmodule* module;
	int clisock;
};

#endif
