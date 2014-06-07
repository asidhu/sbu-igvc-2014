#ifndef _CPUINFO_MODULE_H
#define _CPUINFO_MODULE_H
#include "module.h"
#include "modules/cpuinfo/cpuinfodata.h"
#include <iostream>


#define CPU_DEFAULT_REFRESHRATE 2000



class cpuinfomodule:public module{
private:
	static const char* myName;
	static void* thread(void* arg);
	static void printEvent(std::ostream&, const event*);
	volatile bool m_dataArrived;
	struct cpuinfodata m_cpudata;
	int m_statfd;
	int m_meminfofd;
	int m_netfd;
	volatile bool running;
	long convertMultiplier(char* tmp);
	void readCPU();
	void readMEM();
	void readNET();
	void initializeCPUReader();

	long calculateWork(int core){
		return m_cpudata.cpudata[core].user+m_cpudata.cpudata[core].nice+m_cpudata.cpudata[core].system;
	}

	long calculateTotal(int core){
		int total=0;
		for(int i=0;i<7;i++)
			total+= m_cpudata.cpudata[core].jiffies[i];
		return total;
	}
public:
	int m_refreshrate;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	const char* getCommonName(){
		return myName;
	}
	cpuinfomodule(){
		running=true;
		this->m_refreshrate=CPU_DEFAULT_REFRESHRATE;
		m_dataArrived=false;
		m_cpudata.m_cpus= m_cpudata.m_interfaces=0;
	}

};


#endif
