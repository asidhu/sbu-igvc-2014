#include "arduinoconfig.h"

#include "rapidxml/rapidxml.hpp"
#include <cstring>
#include <cstdio>
#include <dirent.h>
#include "osutils.h"

void readPathConfig(char *cfgfile, char *path) {
  char buff[8192];
  long size = 8192;

  readFile(cfgfile, buff, size);
  
  using namespace rapidxml;
  xml_document<> doc;
  doc.parse<0>(buff);
  xml_node<> * body = doc.first_node();
  xml_node<> * dev = body->first_node("dev", strlen("dev"));
  
  strcpy(path, dev->value());
}
