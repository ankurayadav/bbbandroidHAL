/**********************************************************
  SPI general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "include/linux/spi/spidev.h"
#include "bbbandroidHAL.h"

#define MAX_PATH 50

#define MODE0 0
#define MODE1 1
#define MODE2 2
#define MODE3 3

int spiTransfer(int spiFD, unsigned char tx[], unsigned char rx[], int len)
{
	struct spi_ioc_transfer	transfer;

	transfer.tx_buf = (unsigned long) tx;
	transfer.rx_buf = (unsigned long) rx;
	transfer.len = len;
	transfer.speed_hz = 10000;
	transfer.bits_per_word = 8;

	if(ioctl(spiFD, SPI_IOC_MESSAGE(1), &transfer)<0)
	{
		return -1;
	}

	return 0;
}

unsigned char spiReadByte(int spiFD, unsigned int regAdd)
{
	unsigned char tx[2], rx[2];
	memset(tx, 0, sizeof tx);
	memset(rx, 0, sizeof rx);

	tx[0] = (unsigned char) (0x80 | regAdd);
	
	spiTransfer(spiFD, tx, rx, 2);

	return rx[1];
}

unsigned char* spiReadBytes(int spiFD, unsigned int len, unsigned int startAdd)
{
	unsigned char* data = (unsigned char *)malloc(sizeof(unsigned char)*len);
	unsigned char tx[len+1], rx[len+1];
	memset(tx, 0, sizeof tx);
	tx[0] =  (unsigned char) (0x80 | 0x40 | startAdd);
	
	spiTransfer(spiFD, tx, rx, len+1);
	
	memcpy(data, rx+1, len);
	
	return data;
}

int spiWriteRegByte(int spiFD, unsigned int regAdd, unsigned char data)
{
	unsigned char tx[2], rx[2];
	memset(rx, 0, sizeof rx);

	tx[0] = (unsigned char) regAdd;
	tx[1] = data;

	spiTransfer(spiFD, tx, rx, 2);
	return 0;
}

int spiWriteBytes(int spiFD, unsigned char data[], int len)
{
	unsigned char null = 0x00;
	
	spiTransfer(spiFD, data, &null, len);
	
	return 0;
}

int spiSetMode(int spiFD, uint8_t mode)
{
	if (ioctl(spiFD, SPI_IOC_WR_MODE, &mode)==-1)
	{
		return -1;
	}

	if (ioctl(spiFD, SPI_IOC_RD_MODE, &mode)==-1)
	{
		return -1;
	}

	return 0;
}

int spiSetSpeed(int spiFD, uint32_t speed)
{
	if (ioctl(spiFD, SPI_IOC_WR_MAX_SPEED_HZ, &speed)==-1)
	{
		return -1;
	}

	if (ioctl(spiFD, SPI_IOC_RD_MAX_SPEED_HZ, &speed)==-1)
	{
		return -1;
	}

	return 0;
}

int spiSetBitsPerWord(int spiFD, uint8_t bpw)
{
	if (ioctl(spiFD, SPI_IOC_WR_BITS_PER_WORD, &bpw)==-1)
	{
		return -1;
	}

	if (ioctl(spiFD, SPI_IOC_RD_BITS_PER_WORD, &bpw)==-1)
	{
		return -1;
	}
	return 0;
}

int spiOpen(unsigned int bus, unsigned int device, uint32_t speed, uint8_t mode, uint8_t bpw)    //bpw = pits per word
{
	char fsBuf[MAX_PATH] ;
	int spiFD ;

	snprintf(fsBuf, sizeof(fsBuf), "/dev/spidev%d.%d", bus, device);

	spiFD = open(fsBuf, O_RDWR);
  	
  	if (spiFD < 0) {
    	return -1;
  	}

  	if (spiSetMode(spiFD, mode)==-1) return -1;
	if (spiSetSpeed(spiFD, speed)==-1) return -1;
	if (spiSetBitsPerWord(spiFD, bpw)==-1) return -1;

  	return spiFD ;
}

void spiClose(int spiFD)
{
	close(spiFD);
}