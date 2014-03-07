#ifndef _CPUINFO_DATA_H
#define _CPUINFO_DATA_H
#define _CPUINFO_MAX_CORES 64
#define _CPUINFO_MAX_VALUES 32
#define _CPUINFO_MAX_NET_INTERFACES 16
struct cpuinfodata{
	union{
		long jiffies[_CPUINFO_MAX_VALUES];
		struct{
			long user,nice,system,idle,iowait,irq,softirq;
		};
	}cpudata[_CPUINFO_MAX_CORES+1];
	struct{
		long txByte,rxByte;
		double txkbps, rxkbps;
		char name[64];
	} netdata[_CPUINFO_MAX_NET_INTERFACES];
	double m_cpuUsage[_CPUINFO_MAX_CORES+1];
	double m_memUsage;
	int m_cpus, m_interfaces;
};
#endif
