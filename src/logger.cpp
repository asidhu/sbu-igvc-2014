#include "logger.h"
#include "rapidxml/rapidxml.hpp"
#include "osutils.h"
#include "string.h"
#include <iostream>
#include <stdio.h>
namespace Logger{

const char* defaultLoggerConfig = 
"<Logger>\n\r"
"\t<output>log/console.log</output>\n\r"
"</Logger>\n\r";
const char* myLoggerFile = "log/console.log";


char loggerFile[256];
bool initialized;
};
void Logger::initialize(char* cfgfile){
	char buff[4096];
	long size=4096;
	readFile(cfgfile,buff,size);
	if(size==0){
		size = strlen(defaultLoggerConfig);
		writeFile(cfgfile, defaultLoggerConfig,size);	
		strcpy(loggerFile,myLoggerFile); 
		initialized=true;
		return;
	}
	using namespace rapidxml;
	xml_document<> doc;
	doc.parse<0>(buff);
	const char* nodename = "output";
	xml_node<> *body = doc.first_node();
	xml_node<> *output = body->first_node(nodename,6);
	if(output==NULL)
	{
		std::cout<<"Logger Config error!"<<std::endl;
		initialized=false;
		return;
	}else{
		strcpy(loggerFile,output->value());
		initialized=true;
	}
	
}

void Logger::log(int moduleID, int level, const char* buff, int len){
	if(initialized)
		appendFile(loggerFile, buff, len);
}


void Logger::log(int moduleID, int level, const char* buff){
	char buffer[4096];
	strcpy(buffer,buff);
	char* end = buffer + strlen(buff);
	*end = '\n';
	*(end+1)='\r';
	*(end+2)=0;
	if(initialized)
		appendFile(loggerFile, buffer, strlen(buffer));

	printf(buffer);
}



