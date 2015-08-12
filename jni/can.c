/**********************************************************
  CAN general purpose interface code

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file can.c
 * @author Ankur Yadav (ankurayadav@gmail.com)
 * @brief CAN general purpose interface code
 */

#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include "include/linux/can.h"
#include "include/linux/can/raw.h"
#include "bbbandroidHAL.h"

#ifndef PF_CAN
#define PF_CAN 29	/**< PF_CAN macro defination which was missing from can.h */
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN	/**< AF_CAN macro defination which was missing from can.h */
#endif

/**
 * This function takes socket_type and protocol as input and opens socket of that type and protocol
 * and then returns file descriptor.
 * @param socket_type an integer argument.
 * @param protocol an integer argument.
 * @return If successful then CAN file descriptor after opening socket is returned and if it fails then -1 is returned.
 */

int canOpenSocket(int socket_type, int protocol)
{
	int canFD = socket(PF_CAN, socket_type, protocol);

	if (canFD<0)
	{
		return -1;
	}

	return canFD;
}

/**
 * This function takes port string as input and opens socket using canOpenSocket(),
 * locates interface using ioctl() system call then binds the socket with CAN interface
 * and returns the file descriptor.
 * @param port a constant character pointer argument.
 * @see canOpenSocket()
 * @return If successful then CAN file descriptor after opening socket is returned and if it fails then -1 is returned.
 */

int canOpenRaw(const char *port)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int canFD = canOpenSocket(SOCK_RAW, CAN_RAW);  /* Creating socket for can */

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, port);

	if (ioctl(canFD, SIOCGIFINDEX, &ifr) < 0)    /* Locating the interface (ifr.ifr_ifindex gets filled with device index) */
    {
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;  

    if (bind(canFD, (struct sockaddr *)&addr, sizeof(addr)) < 0) /* Bind the socket with can interface */
    {
        return (-1);
    }

	return canFD;
}

/**
 * This functions takes file descriptor and length of data to be read as input
 * and returns the pointer to the array of data bytes read. If no data is read then NULL pointer is returned.
 * @param fd an integer argument.
 * @param length an integer pointer argument.
 * @return The pointer to the array of data bytes read. If no data is read then NULL pointer is returned.
 */

unsigned char* canReadBytes(int fd, int *length)
{
	struct can_frame frame;

	struct timeval timeout = {1,0};
	
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);

	if(select((fd + 1), &set, NULL, NULL, &timeout) >= 0)
	{
		if(FD_ISSET(fd, &set))
		{
			read(fd, &frame, sizeof(struct can_frame));

			unsigned char* data = (unsigned char *)malloc(sizeof(unsigned char)*(frame.can_dlc+1));

    		data[0] = frame.data[0];
    		int i=1;
			while(i<frame.can_dlc)
			{
				data[i] = frame.data[i];
				i++;
			}

			*length = frame.can_dlc;

		    return data;
		}
	}

	*length = 0;

	return NULL;
}

/**
 * This function takes can file descriptor, number of bytes and array of data to be transferred
 * as input and writes given number of bytes of data from data buffer.
 * @param canFD an integer argument.
 * @param no_bytes an unsigned integer argument.
 * @param data array of unsigned char argument.
 * @return number of bytes successfully written to CAN device.
 */

int canSendBytes(int canFD, unsigned int no_bytes, unsigned char data[])
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

	int nbytes = write(canFD, &frame, sizeof(struct can_frame));

	return nbytes;
}

/**
 * This function is used to close CAN file descriptor.
 * @param canFD an integer argument.
 */

void canClose(int canFD)
{
	close(canFD);
}