#ifndef _OSUTILS_H
#define _OSUTILS_H

int spawnThread( void*(*threaded)(void *), void * arg);
int openSerialPort( const char* serialport, int speed, int parity, int blocking);


#endif
