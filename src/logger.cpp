#include "logger.h"
#include "rapidxml/rapidxml.hpp"
#include "osutils.h"
#include "string.h"
#include <iostream>
#include <stdio.h>
#include "module.h"
namespace Logger{

const char* defaultLoggerConfig = 
"<Logger>\n\r"
"\t<output>log/console.log</output>\n\r"
"</Logger>\n\r";
const char* myLoggerFile = "log/console.log";


char loggerFile[256];
bool initialized;
std::vector<module*> *m_modules;
};
void Logger::initialize(char* cfgfile, std::vector<module*> *modules){
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
	m_modules=modules;
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




void Logger::log(int moduleID, int level, const char* format, ...){
	va_list a_list;
	char buffer[4096], buffer2[4096];
	va_start(a_list,format);
	vsnprintf(buffer,4096,format,a_list);
	va_end(a_list);
	snprintf(buffer2,4096,"%s: %s \n\r", m_modules->at(moduleID)->getCommonName(),buffer);
	if(initialized)
		appendFile(loggerFile, buffer2, strlen(buffer2));

	printf(buffer2);
}



