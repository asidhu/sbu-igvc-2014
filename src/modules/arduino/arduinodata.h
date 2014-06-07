#ifndef _ARDUINO_DATA_H
#define _ARDUINO_DATA_H
#define MAX_LINE_LENGTH	1024
#define FLAG_RESET_MOTORS	0x0001
#define FLAG_LEDS_FLASH		0x0002
#define FLAG_LEDS_ON		0x0004
#define FLAG_LEDS_OFF		0x0008
struct arduinodata{
	char data[MAX_LINE_LENGTH];
	int numChar;
	int eflag;
};
struct arduinocmd{
	int arduino_flag;	
};

#endif
