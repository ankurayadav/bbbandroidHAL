/**********************************************************
  I2C general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file i2c.c
 * @author Ankur Yadav (ankurayadav@gmail.com)
 * @brief I2C general purpose interface code for file system
 */

#include <stdio.h>
#include <fcntl.h> 
#include <sys/ioctl.h>
#include "include/linux/i2c-dev.h"
#include "bbbandroidHAL.h"

#define MAX_PATH 50	/**< Maximum buffer for creating path using snprintf() */

/**
 * This function takes I2C adapter number as input and returns a file descriptor for accessing that I2C adapter.
 * @param adaptorNumber an uint8_t argument.
 * @return If successful then I2C file descriptor is returned and if it fails then -1 is returned.
 */

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

/**
 * This takes I2C adapter file descriptor and address at which device is attached as input
 * and sets slave for file descriptor using ioctl call.
 * @param i2cFD an integer argument.
 * @param address an uint8_t argument.
 * @return If successful then 0 is returned and if it fails then -1 is returned.
 */

int i2cSetSlave(int i2cFD, uint8_t address)
{
	if (ioctl(i2cFD, I2C_SLAVE, address) < 0) 
	{
		return -1;	
	}

	return 0;
}

/**
 * This is used by some other functions internally for random access of sensor registers.
 * It takes file descriptor and random address as input to which file descriptor should point.
 * @param i2cFD an integer argument.
 * @param add an unsigned character argument.
 * @return 0 on success and -1 if it fails.
 */

int i2cSetAddress(int i2cFD, unsigned char add)
{
   	if (i2c_smbus_write_byte(i2cFD, add) < 0)
   	{
   	   return -1;
   	}
   	
   	return 0;
}

/**
 * This function takes file descriptor, address of register to write
 * and byte to be written as input and writes byte provided in the argument at the address provided.
 * @param i2cFD an integer argument.
 * @param add an unsigned character argument.
 * @param byte an unsigned character argument.
 * @return 0 on success and -1 if it fails.
 */

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

/**
 * It takes i2c file descriptor, start address of registers to be written, length of data and array containing bytes of data as input.
 * This function is used to write bytes provided in argument from the address provided to address+length continuously.
 * @param i2cFD an integer argument.
 * @param add an unsigned character argument.
 * @param length an integer argument.
 * @param bytes an uint8_t pointer argument.
 * @return 0 on success and -1 if it fails.
 */

int i2cWriteBytes(int i2cFD, unsigned char add, int length, uint8_t *bytes)
{
	if(i2c_smbus_write_i2c_block_data(i2cFD, add, length, bytes) < 0)
	{
		return -1;
	}

	return 0;
}

/**
 * It takes file descriptor and address of register from which data is to be read.
 * It reads a byte from the address provided in input and returns the byte.
 * It internally uses i2cSetAddress() to set pointer to proper address.
 * @param i2cFD an integer argument.
 * @param add an unsigned character argument.
 * @see i2cSetAddress()
 * @return byte on success and -1 if it fails.
 */

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

/**
 * It takes file descriptor, starting address of register from which data is to be read, length of 
 * data to be read and buffer to store the data as input.
 * It is used to get 'length' number of bytes in 'buff' array from 'add' starting address.
 * @param i2cFD an integer argument.
 * @param add an unsigned character argument.
 * @param length an integer argument.
 * @param bytes an uint8_t pointer argument.
 * @return 0 on success and -1 if it fails.
 */

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

/**
 * This function is used to close I2C file descriptor.
 * @param i2cFD an integer argument.
 */

void i2cClose(int i2cFD)
{
	close(i2cFD);
	
	i2cFD = -1;
}