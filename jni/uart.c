/**********************************************************
  UART general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include "bbbandroidHAL.h"

#define MAX_PATH 50

static speed_t baudrate(int baudrate)
{
	switch(baudrate)
	{
		case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
	}

	return -1;
}

int uartOpen(unsigned int device, unsigned int bdrate)
{
	char fsBuf[MAX_PATH] ;
	int uartFD ;

	snprintf(fsBuf, sizeof(fsBuf), "/dev//dev/ttyO%d", device);

	uartFD = open(fsBuf, O_RDWR | O_NOCTTY | O_NDELAY);

	if(uartFD == -1)
	{
  		return -1;	
	}

	speed_t baud = baudrate(bdrate);
	if(baud==-1)
	{
		return -1;
	}

	struct termios config;
	if(tcgetattr(uartFD, &config))
	{
		return -1;
	}

	cfmakeraw(&config);
    cfsetispeed(&config, baud);
    cfsetospeed(&config, baud);

    tcflush(uartFD, TCIFLUSH); 

    if(tcsetattr(uartFD, TCSANOW, &config))
	{
		return -1;
	}

	return uartFD;
}

int uartWrite(int uartFD, int length, unsigned char *bytes)
{  
	if(write(uartFD, bytes, length)<0)
	{
		return -1;
	}

	return 0;
}

int uartRead(int uartFD, int length, unsigned char *bytes)
{  
	if(read(uartFD, bytes, length)<0)
	{
		return -1;
	}

	return 0;
}

void uartClose(int uartFD)
{
	close(uartFD);
}