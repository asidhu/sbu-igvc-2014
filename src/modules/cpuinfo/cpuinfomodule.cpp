#include "modules/cpuinfo/cpuinfomodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include "event_flag.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
	void cpuinfomodule::readCPU(){
		//m_statfd = open("/proc/stat",O_RDONLY);
		cpuinfodata* d = & m_cpudata;
		lseek(m_statfd,0,SEEK_SET);
		char buffer[1024];
		int numBytes=read(m_statfd,buffer,1024);
		if(numBytes<=0)return;
		buffer[numBytes]=0;
		int currCPU=0;
		long total =0, working=0;
		total = calculateTotal(currCPU);
		working = calculateWork(currCPU);
		sscanf(buffer,"cpu %ld %ld %ld %ld %ld %ld %ld",d->cpudata[currCPU].jiffies, d->cpudata[currCPU].jiffies+1,
				d->cpudata[currCPU].jiffies+2, d->cpudata[currCPU].jiffies+3, d->cpudata[currCPU].jiffies+4,
				d->cpudata[currCPU].jiffies+5, d->cpudata[currCPU].jiffies+6);	
		total= calculateTotal(currCPU)-total;
		working = calculateWork(currCPU)-working;
		d->m_cpuUsage[currCPU] = (((double)working)/total)*100;
		char* loc = (char*)strchr(buffer,'\n')+1;
		currCPU++;
		total = calculateTotal(currCPU);
		working = calculateWork(currCPU);
		int tst;
		while((tst=sscanf(loc,"cpu%*d %ld %ld %ld %ld %ld %ld %ld",d->cpudata[currCPU].jiffies, d->cpudata[currCPU].jiffies+1,
				d->cpudata[currCPU].jiffies+2,d->cpudata[currCPU].jiffies+3,d->cpudata[currCPU].jiffies+4,
				d->cpudata[currCPU].jiffies+5,d->cpudata[currCPU].jiffies+6))!=EOF && tst!=0){
			total= calculateTotal(currCPU)-total;
			working = calculateWork(currCPU)-working;
			
			loc = (char*)strchr(loc,'\n')+1;
			d->m_cpuUsage[currCPU] = (((double)working)/total)*100;
			currCPU++;
			if(currCPU>=_CPUINFO_MAX_CORES)break;
			total = calculateTotal(currCPU);
			working = calculateWork(currCPU);
		}
		d->m_cpus=currCPU;	
		
	}
	long cpuinfomodule::convertMultiplier(char* tmp){
		if(tmp[0]=='B')
			return 1;//oh well?
		else if(tmp[0]=='k')
			return 1024;
		else if(tmp[0]=='m')
			return 1024*1024;
		else if(tmp[0]=='g')
			return 1024*1024*1024;
		else if(tmp[0]=='t')
			return 1024L*1024L*1024L*1024L;
		return 1;
	}
	void cpuinfomodule::readMEM(){
		lseek(m_meminfofd,0,SEEK_SET);
		char buffer[1024];
		int nBytes = read(m_meminfofd,buffer,1024);
		if(nBytes<=0)return;
		buffer[nBytes]=0;
		long total, free;
		char tmp[64];
		sscanf(buffer,"%*s %li %63s\n",&total,(char*)&tmp);
		total*= convertMultiplier(tmp);		
		char* loc = (char*)strchr(buffer,'\n')+1;
		sscanf(loc,"%*s %li %63s\n",&free,(char*)&tmp);
		free*= convertMultiplier(tmp);		
		m_cpudata.m_memUsage = (1-(double)free/total)*100;

	}

	void cpuinfomodule::readNET(){
		lseek(m_netfd,0,SEEK_SET);
		char buffer[1024];
		int nBytes = read(m_netfd,buffer,1024);
		if(nBytes<=0)return;
		buffer[nBytes]=0;
		long newTx, newRx;
		char* loc = (char*)strchr(buffer,'\n'); //read header
		loc = (char*)strchr(loc+1,'\n'); //read header
		int numinterfaces=0;
		while(loc!=NULL){
			int match =sscanf(loc+1, "%63s %ld %*d %*d %*d %*d %*d %*d %*d %ld",m_cpudata.netdata[numinterfaces].name,
				&(newRx),&(newTx));
			if(match<3)break;
			double bandTx = newTx - m_cpudata.netdata[numinterfaces].txByte,bandRx= newRx - m_cpudata.netdata[numinterfaces].rxByte;
			m_cpudata.netdata[numinterfaces].txkbps = bandTx/1024 /(this->m_refreshrate/1000);
			m_cpudata.netdata[numinterfaces].rxkbps = bandRx/1024 /(this->m_refreshrate/1000);
			m_cpudata.netdata[numinterfaces].txByte=newTx;
			m_cpudata.netdata[numinterfaces].rxByte=newRx;
			loc = (char*)strchr(loc+1,'\n'); //skip line
			numinterfaces++;
		}
		m_cpudata.m_interfaces=numinterfaces;
	}

	void* cpuinfomodule::thread(void* args){
		cpuinfomodule* module = (cpuinfomodule*) args;
		while(true){
			sleepms(module->m_refreshrate);
			module->readCPU();
			module->readMEM();
			module->readNET();
			module->m_dataArrived=true;
		}
		return NULL;			
	}

	void cpuinfomodule::initializeCPUReader(){
		spawnThread(cpuinfomodule::thread, this);
		m_statfd = open("/proc/stat",O_RDONLY);
		m_meminfofd = open("/proc/meminfo",O_RDONLY);
		m_netfd = open("/proc/net/dev",O_RDONLY);
		m_dataArrived=false;
	}
	/**
		rules of module ettiquette:
		1) only create a thread if you are reading from a file, doing io actions, socket ops, sleeping, blocking OR intensive calculations being performed. (AKA camera manipulation operations, intense geometric operations, anything greater than 5 ms worth of time)
		2) all threads should be monitored and kept track of and cleaned up when asked to.
		3) all modules should respond to standard commands.
	**/
	
	/**
		Rules:
		1) FEEL FREE to read information in from files. 
		2) If this is like the camera, and a command comes in to connect to camera, dont read configuration than. Load configuration NOW.
		3) dont open hardware or pipes or sockets now, there is a command for that. Open it at that time, only read files.
		4) listeners should also be setup HERE. If you want to listen to some event, take listener_flag and | it with the event flag.
	**/
	void cpuinfomodule::initialize(uint32& listener_flag){
		initializeCPUReader();
	}

	void cpuinfomodule::printEvent(std::ostream& out, const event* evt){
		cpuinfodata* cpu = (cpuinfodata*)(evt->m_data);
		out<< "cpuinfo:CPU:"<<cpu->m_cpus<<std::endl;
		for(int i=0;i<cpu->m_cpus;i++){
			if(i)
				out<<"C"<<i<<":";
			else
				out<<"Total:";
			out<<cpu->m_cpuUsage[i]<<"%"<<std::endl;
		}
		out<< "cpuinfo:MEM:"<<cpu->m_memUsage<<"%"<<std::endl;


		out<<"cpuinfo:NET: "<<cpu->m_interfaces<<std::endl;
		for(int i=0;i<cpu->m_interfaces;i++){
			out<< cpu->netdata[i].name<<":";
			out<<"TX:"<<cpu->netdata[i].txkbps<<"kbps ";
			out<<"RX:"<<cpu->netdata[i].rxkbps<<"kbps"<<std::endl;
		}
	}
	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void cpuinfomodule::update(bot_info* data){
		if(m_dataArrived){
			event* evt = this->makeEvent(EFLAG_CPUDATA,&m_cpudata);
			evt->m_print= cpuinfomodule::printEvent;
			data->m_eventQueue.push_back(evt);
			m_dataArrived=false;
		}
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void cpuinfomodule::pushEvent(event* evt){
		
	}

const char* cpuinfomodule::myName="CPUInfo Module";
