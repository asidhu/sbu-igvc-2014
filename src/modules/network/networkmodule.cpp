#include "networkmodule.h"
#include "event.h"
#include "osutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
	/**
		rules of module ettiquette:
		1) only create a thread if you are reading from a file, doing io actions, socket ops, sleeping, blocking OR intensive calculations being performed. (AKA camera manipulation operations, intense geometric operations, anything greater than 5 ms worth of time)
		2) all threads should be monitored and kept track of and cleaned up when asked to.
		3) all modules should respond to standard commands.
	**/
	
	void* networkmodule::threadserver(void* mod){
		networkmodule* module = (networkmodule*)mod;
		module->serverlisten();
		return NULL;
	}

	void* networkmodule::threadclient(void* mod){
		cli_handlerinfo* nfo = (cli_handlerinfo *)mod;
		nfo->module->clienthandle(nfo->clisock);
		delete nfo;
		return NULL;
	}

	void networkmodule::clienthandle(int sock){

	}
	void networkmodule::serverlisten(){
		int serversockfd, clisockfd, portno;
		socklen_t clilen;
		struct sockaddr_in serv_addr, cli_addr;
		serversockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(serversockfd<0){
			openSockFailure("cannot create socket.");
			return;
		}
		bzero((char*) &serv_addr,sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if(bind(serversockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
			openSockFailure("cannot bind socket.");
			return;
		}
		listen(serversockfd,5);
		
		while(1){
			clilen = sizeof(cli_addr);
			clisockfd = accept(serversockfd,(struct sockaddr*)&cli_addr,&clilen);
			if(clisockfd<0){
				openSockFailure("Error accepting client socket!");
				return;
			}
			cli_handlerinfo* data = (cli_handlerinfo*)malloc(sizeof(cli_handlerinfo));	
			data->module=this;
			data->clisock = clisockfd;
			spawnThread(networkmodule::threadclient, data);
		}
	}

	/**
		Rules:
		1) FEEL FREE to read information in from files. 
		2) If this is like the camera, and a command comes in to connect to camera, dont read configuration than. Load configuration NOW.
		3) dont open hardware, there is a command for that. Open it at that time, only read files.
		4) listeners should also be setup HERE. If you want to listen to some event, take listener_flag and | it with the event flag.
	**/
	void networkmodule::initialize(uint32& listener_flag){
		spawnThread(networkmodule::threadserver,this);
	}


	/**
		Rules:
		1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
		2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
		3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
	**/
	void networkmodule::update(bot_info* data){

	}
	/**
		Rules:
		1) this is if an event is pushed to you.
		2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
	**/
	void networkmodule::pushEvent(event* evt){
		
	}
const char* networkmodule::myName="Network Module";
