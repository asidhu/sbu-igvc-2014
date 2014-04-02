#ifndef _OSUTILS_H
#define _OSUTILS_H

int spawnThread( void*(*threaded)(void *), void * arg);
int openSerialPort( const char* serialport, int speed, int parity, int blocking);
void sleepms(int ms);
void readFile(const char* filename, char* buff, long& size);
void writeFile(const char* filename, const char* buff, const long size);
void appendFile(const char* filename, const char* buff, const long size);


#endif
