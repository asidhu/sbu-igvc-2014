#include "debugmodule.h"
#include "event.h"
#include "base.h"
void debugmodule::initialize(uint32& flag){
	flag=0xFFFFFFFF;
}

void debugmodule::update(bot_info* data){
	//this only works if debug module is at the end.. geez.


	//lets check the times each module took to complete and report any offenders.	
	for(uint32 i=0;i<data->m_moduleTimerLen;i++){
		if(data->m_moduleTimer[i]>WARN_MODULE_TIMER){
			*m_output<< myName <<": module has exceeded the recommended execution time. time:"<<data->m_moduleTimer[i]
				<< " us, module: " << data->m_modules->at(i)->getCommonName()<<"(id="<<i<<")"<<std::endl;
		}
		//*m_output<< myName << ": module " << data->m_modules->at(i)->getCommonName() << " took " << data->m_moduleTimer[i] << 
		//	" us."<<std::endl;
	}
}

void debugmodule::pushEvent(event* evt){
	*m_output<<myName<<":"<<evt;
}

const char* debugmodule::myName="Debug Module";
