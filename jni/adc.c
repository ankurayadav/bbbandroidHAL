/**********************************************************
  ADC general purpose interface code for raw voltage input 
  from file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include <stdint.h>
#include "bbbandroidHAL.h"

#define SYSFS_ADC_DIR "/sys/bus/iio/devices/iio:device0"

static char fsBuf[100];

uint32_t readADC(unsigned int channel)
{
	FILE *fd;
	int value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_ADC_DIR "/in_voltage%d_raw", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return -1;
	}

  	fscanf(fd, "%d", &value);

  	fclose(fd);

  	return value;
}