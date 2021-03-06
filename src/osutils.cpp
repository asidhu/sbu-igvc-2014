#include "osutils.h"
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

void sleepms(int ms){
	usleep(ms*1000);
}


void readFile(const char* name, char* data, long& size){
	if(size ==0 || data==NULL)
	{
		FILE* file;
		file = fopen(name, "rb");
		if(file == NULL){
			size=0;
			data=NULL;
			return;
		}
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fclose(file);
		return;
	}
	FILE * file;
	file = fopen(name, "rb");
	if(file==NULL){
		size = 0;
		return;
	}
	long result = fread(data,1,size,file);
	size = result;
	data[result]=0;
	fclose(file);
}

void writeFile( const char* name, const char* data,const long size){
	FILE* file = fopen(name, "wb");
	fwrite(data,1,size,file);
	fclose(file);
}

void appendFile(const char* name,const char* data,const long size){
	FILE* file = fopen(name, "ab");
	fwrite(data,1,size,file);
	fclose(file);
}

int spawnThread( void*(*threaded)(void*), void * arg){
	pthread_t thread;
	pthread_attr_t attr;
	if(pthread_attr_init(&attr))
		return 0;
	if(pthread_create(&thread,&attr,threaded, arg))
		return 0;
	else
		return 1;
}

int set_interface_attribs (int fd, int speed, int parity, int c_flag)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_cflag &= ~CS8;
	tty.c_cflag |=c_flag;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0);
                //error_message ("error %d setting term attributes", errno);
}


int openSerialPort( const char* serialport, int speed, int parity, int blocking,	int c_flag){
	speed_t actualSpeed;
	if(speed==9600)
		actualSpeed= B9600;
	else if(speed==19200)
		actualSpeed= B19200;
	else if(speed==38400)
		actualSpeed= B38400;
	else if(speed==57600)
		actualSpeed= B57600;
	else if(speed==115200)
		actualSpeed=B115200;
	else
		actualSpeed=speed;	
	int fd = open (serialport, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
	        //error_message ("error %d opening %s: %s", errno, serialport, strerror (errno));
	        return -1;
	}

	set_interface_attribs (fd, actualSpeed, parity,c_flag);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, blocking);                // set no blocking
	return fd;
}



