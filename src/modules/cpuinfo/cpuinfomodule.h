#ifndef _CPUINFO_MODULE_H
#define _CPUINFO_MODULE_H
#include "module.h"
#define MAX_CORES 64
#define MAX_VALUES 32
#define MAX_NET_INTERFACES 16
#define CPU_INFO_SLEEP 2000
class cpuinfomodule:public module{
	private:
	static const char* myName;
	public:
	int m_cpus;
	int m_statfd;
	int m_meminfofd;
	int m_netfd;
	double m_cpuUsage[MAX_CORES+1];
	double m_memUsage;
	void initialize(uint32&);
	void update(bot_info*);
	void pushEvent(event* );
	void readCPU();
	void readMEM();
	void readNET();
	int convertMultiplier(char* tmp);
	void initializeCPUReader();
	static void* thread(void* arg);
	const char* getCommonName(){
		return myName;
	}
	union{
		long jiffies[MAX_VALUES];
		struct{
			long user,nice,system,idle,iowait,irq,softirq;
		};
	}cpudata[MAX_CORES+1];
	struct{
		long txByte,rxByte;
		double txkbps, rxkbps;
		char name[64];
	} netdata[MAX_NET_INTERFACES];
	long txBytes[MAX_NET_INTERFACES], rxBytes[MAX_NET_INTERFACES];
	long calculateWork(int core){
		return cpudata[core].user+cpudata[core].nice+cpudata[core].system;
	}
	long calculateTotal(int core){
		int total=0;
		for(int i=0;i<7;i++)
			total+= cpudata[core].jiffies[i];
		return total;
	}
};


#endif
