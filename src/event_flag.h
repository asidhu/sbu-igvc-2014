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


#endif
