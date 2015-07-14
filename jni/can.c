/**********************************************************
  CAN general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include "include/linux/can.h"
#include "include/linux/can/raw.h"
#include "bbbandroidHAL.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

int canOpenSocket(int socket_type, int protocol)
{
	int fd = socket(PF_CAN, socket_type, protocol);

	if (fd<0)
	{
		return -1;
	}

	return fd;
}

int canOpenRaw(char *port)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int fd = canOpenSocket(SOCK_RAW, CAN_RAW);  /* Creating socket for can */

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, port);

	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)    /* Locating the interface (ifr.ifr_ifindex gets filled with device index) */
    {
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;  

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) /* Bind the socket with can interface */
    {
        return (-1);
    }

	return fd;
}

unsigned char* canReadBytes(int fd)
{
	struct can_frame frame;

    read(fd, &frame, sizeof(struct can_frame));

    unsigned char* data = (unsigned char *)malloc(sizeof(unsigned char)*(frame.can_dlc+1));

    data[0] = frame.data[0];
    int i=1;
	while(i<frame.can_dlc)
	{
		data[i] = frame.data[i];
		i++;
	}

    return data;
}

int canSendBytes(int fd, unsigned char data[], unsigned int no_bytes)
{
	struct can_frame frame;
	frame.can_id = 0x123;
	//strcpy( frame.data, data );
	//frame.can_dlc = strlen( frame.data );

	int i=0;
	while(i<no_bytes)
	{
		frame.data[i] = data[i];
		i++;
	}

	frame.can_dlc = no_bytes;

	int nbytes = write(fd, &frame, sizeof(struct can_frame));

	return nbytes;
}

void canClose(int fd)
{
	close(fd);
}