/**********************************************************
  I2C general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/
#include <stdio.h>
#include <fcntl.h> 
#include <sys/ioctl.h>
#include "include/linux/i2c-dev.h"
#include "bbbandroidHAL.h"

#define MAX_PATH 50

int i2cOpenAdaptor(uint8_t adaptorNumber)
{
	char fsBuf[MAX_PATH] ;
	int i2cFD ;

	snprintf(fsBuf, sizeof(fsBuf), "/dev/i2c-%d", adaptorNumber);

	i2cFD = open(fsBuf, O_RDWR);
  	
  	if (i2cFD < 0) {
    	return -1;
  	}

  	return i2cFD ;
}


int i2cSetSlave(int i2cFD, uint8_t address)
{
	if (ioctl(i2cFD, I2C_SLAVE, address) < 0) 
	{
		return -1;	
	}

	return 0;
}

int i2cSetAddress(int i2cFD, unsigned char add)
{
   	if (i2c_smbus_write_byte(i2cFD, add) < 0)
   	{
   	   return -1;
   	}
   	
   	return 0;
}

int i2cWriteByte(int i2cFD, unsigned char add, unsigned char byte)
{
	unsigned char buff[2];
   	
   	buff[0] = add;
   	buff[1] = byte;
   
	if(write(i2cFD, buff, 2)!=2)
	{
		return -1;
	}

	return 0;
}

int i2cWriteBytes(int i2cFD, unsigned char add, int length, uint8_t *bytes)
{
	if(i2c_smbus_write_i2c_block_data(i2cFD, add, length, bytes) < 0)
	{
		return -1;
	}

	return 0;
}

int i2cReadByte(int i2cFD, unsigned char add)
{
	int byte;

	i2cSetAddress(i2cFD, add);

	if ((byte = i2c_smbus_read_byte(i2cFD)) < 0) 
	{
		return -1;
	}

	return byte;
}

int i2cReadBytes(int i2cFD, unsigned char add, int length, uint8_t *buff)
{
	if(i2c_smbus_read_i2c_block_data(i2cFD, add, length, buff) < 0)
	{	
		return -1;
	}
	else
	{
		return 0;
	}

}

void i2cClose(int i2cFD)
{
	close(i2cFD);
	
	i2cFD = -1;
}