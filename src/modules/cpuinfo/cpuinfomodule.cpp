#include "modules/cpuinfo/cpuinfomodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
	void cpuinfomodule::readCPU(){
		//m_statfd = open("/proc/stat",O_RDONLY);
		lseek(m_statfd,0,SEEK_SET);
		char buffer[1024];
		int numBytes=read(m_statfd,buffer,1024);
		if(numBytes<=0)return;
		buffer[numBytes]=0;
		int currCPU=0;
		int len;
		long total =0, working=0;
		total = calculateTotal(currCPU);
		working = calculateWork(currCPU);
		sscanf(buffer,"cpu %d %d %d %d %d %d %d",cpudata[currCPU].jiffies, cpudata[currCPU].jiffies+1,
				cpudata[currCPU].jiffies+2,cpudata[currCPU].jiffies+3,cpudata[currCPU].jiffies+4,
				cpudata[currCPU].jiffies+5,cpudata[currCPU].jiffies+6);	
		total= calculateTotal(currCPU)-total;
		working = calculateWork(currCPU)-working;
		m_cpuUsage[currCPU] = (((double)working)/total)*100;
		std::cout<<"Working "<<working<<"total "<<total<<" CPU Usage:"<< m_cpuUsage[currCPU]<< std::endl;
		char* loc = (char*)strchr(buffer,'\n')+1;
		currCPU++;
		total = calculateTotal(currCPU);
		working = calculateWork(currCPU);
		int tst;
		while((tst=sscanf(loc,"cpu%*d %d %d %d %d %d %d %d",cpudata[currCPU].jiffies, cpudata[currCPU].jiffies+1,
				cpudata[currCPU].jiffies+2,cpudata[currCPU].jiffies+3,cpudata[currCPU].jiffies+4,
				cpudata[currCPU].jiffies+5,cpudata[currCPU].jiffies+6))!=EOF && tst!=0 && currCPU<MAX_CORES){
			total= calculateTotal(currCPU)-total;
			working = calculateWork(currCPU)-working;
			
			loc = (char*)strchr(loc,'\n')+1;
			m_cpuUsage[currCPU] = (((double)working)/total)*100;
			std::cout<<"Working "<<working<<"total "<<total<<" CPU Usage:"<< m_cpuUsage[currCPU]<< std::endl;
			currCPU++;
			total = calculateTotal(currCPU);
			working = calculateWork(currCPU);
		}
		
		
	}

	void* cpuinfomodule::thread(void* args){
		cpuinfomodule* module = (cpuinfomodule*) args;
		while(true){
			sleepms(2000);
			module->readCPU();
		}			
	}

	void cpuinfomodule::initializeCPUReader(){
		m_statfd = open("/proc/stat",O_RDONLY);
		spawnThread(cpuinfomodule::thread, this);
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


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void cpuinfomodule::update(bot_info* data){
	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void cpuinfomodule::pushEvent(event* evt){
		
	}

const char* cpuinfomodule::myName="CPUInfo Module";
