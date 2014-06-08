#ifndef _EVENT_FLAG_H
#define _EVENT_FLAG_H

/**
	This header defines flag bits. More documentation can be found in report or on the drive.
	Flags are 32 bit. Define all event flags HERE. you shouldn't define a flag in your own header.
**/
//RESERVE 0x1* to 0xF* FOR COMMANDS
#define EFLAG_TERMINATE		0x10000000


//CPUINFO Module Flags
#define EFLAG_CPUDATA		0x01000000


//ARDUINO MODULE
#define EFLAG_ARDUINORAW	0x00100000
#define EFLAG_ARDUINOCMD	0x00200000
//GPS Module Flags
#define EFLAG_GPSDATA		0x00400000
#define EFLAG_GPSRAW		0x00800000

//IMU Module Flags
#define EFLAG_IMURAW		0x00010000
#define EFLAG_IMUDATA		0x00020000


//Joystick Module Flags
#define EFLAG_JOYSTICKEVT	0x00040000

#endif
