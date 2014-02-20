#ifndef _QUERY_H
#define _QUERY_H
/**
	lists all query and response code. Should also include common data structures.
**/

//General queries - most modules should respond or comply
#define QUERY_READY		0x00000001
//this turns on functionality of modules. This does not mean robot should start moving ( referring to ai module or motor module)
//this means you should connect to any device or resource you need in order to function

//Response is usually RESPONSE_OK


#define QUERY_BEGIN_MONITOR	0x00000002
//This means sensor modules should actively start monitoring hardware (i.e. cameras start processing and obj detection, gps starts location reporting, imu starts measuring compass info, etc)

#define QUERY_STOP_MONITOR	0x00000003
//This is opposite of the one above.

#define QUERY_STATUS_UPDATE	0x00000004
//This asks modules for status updates. AKA give me all information you can (NOT ALL SENSOR DATA) just up status, or fps, or battery voltage, etc.

//More to come


//Responses

#define RESPONSE_ACK		0x00000001
//this is an ok response its like okay ill do what you want but it will take more time.

#define RESPONSE_BUSY		0x00000002
//I'm too busy doing something else to do what you want....

#define RESPONSE_SUCCESS	0x00000003
//I succeeded in doing what you asked...

#define RESPONSE_FAILURE	0x00000004
//I FAILED... miserably :( 
#endif
