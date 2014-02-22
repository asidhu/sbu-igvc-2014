#ifndef _CPUINFO_MODULE_H
#define _CPUINFO_MODULE_H
#include "module.h"
#define MAX_CORES 64
#define MAX_VALUES 32
class cpuinfomodule:public module{
	private:
	static const char* myName;
	public:
	int m_cpus;
	int m_statfd;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	void readCPU();
	void initializeCPUReader();
	static void* thread(void* arg);
	const char* getCommonName(){
		return myName;
	}
	union{
		int jiffies[MAX_VALUES];
		struct{
			int user,nice,system,idle,iowait,irq,softirq;
		};
	}cpudata[MAX_CORES+1];
	double m_cpuUsage[MAX_CORES+1];
	int calculateWork(int core){
		return cpudata[core].user+cpudata[core].nice+cpudata[core].system;
	}
	int calculateTotal(int core){
		int total=0;
		for(int i=0;i<7;i++)
			total+= cpudata[core].jiffies[i];
		return total;
	}
};


#endif
