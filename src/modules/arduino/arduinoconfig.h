#ifndef _ARDUINO_CONFIG_H
#define _ARDUINO_CONFIG_H

#define DIRPATH "/dev"
#define DEVROOT ttyACM

void readPathConfig(char *cfgfile, char *path);

void findDev(char *path);

#endif
