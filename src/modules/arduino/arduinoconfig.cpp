#include "arduinoconfig.h"

#include "rapidxml/rapidxml.hpp"
#include <cstring>
#include <cstdio>

char * readPathConfig(int ardNum, int m_id) {
  char path[256];
  char buff[8192];
  
  if (ardNum < 1 || ardNum > 2) {
    Logger::log(m_id, LOGGER_ERROR, "Invalid arduino number referenced");
    return path;
  }

  char filename[256];
  char arduinoName[256];
  sprintf(filename, "arduino%d.cfg", ardNum);
  sprintf(arduinoName, "Arduino%d", ardNum);

  using namespace rapidxml;
  xml_document<> doc;
  doc.parse<0>(buff);
  xml_node<> * body = doc.first_node(arduinoName, strlen(arduinoName));
  xml_node<> * dev = body->first_node("dev", strlen("dev"));
  
  strcpy(path, dev->value());

  return path;
}
