#ifndef _ARDUINO_DATA_H
#define _ARDUINO_DATA_H
#define MAX_LINE_LENGTH	1024
struct arduinodata{
	char data[MAX_LINE_LENGTH];
	int numChar;
	int eflag;
};
#endif
