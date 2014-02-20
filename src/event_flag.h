#ifndef _EVENT_FLAG_H
#define _EVENT_FLAG_H

/**
	This header defines flag bits. More documentation can be found in report or on the drive.
	Flags are 32 bit.
**/
//first byte is checked for equality and are dispatched under special conditions or to all modules
#define TERMINATE_EVENT_FLAG		0x10000000
#define QUERY_EVENT_FLAG		0x20000000
#define RESPONSE_EVENT_FLAG		0x30000000
//lets reserve 2 bytes for sensor events
#define SENSOR_EVENT_FLAG		0x01000000
#define ORIENTATION_EVENT_FLAG		0x02000000
#define LOCALIZATION_EVENT_FLAG		0x04000000
#define OBSTACLE_EVENT_FLAG		0x08000000

// rest of the flags
#define STATUS_EVENT_FLAG		0x00010000
#endif
