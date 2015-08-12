/**********************************************************
  ADC general purpose interface code for raw voltage input 
  from file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file adc.c
 * @author Ankur Yadav (ankurayadav@gmail.com)
 * @brief   ADC general purpose interface code for raw voltage input from file system
 */

#include <stdio.h>
#include <stdint.h>
#include "bbbandroidHAL.h"

#define SYSFS_ADC_DIR "/sys/bus/iio/devices/iio:device0" /**< File system path to access ADC */

static char fsBuf[100]; /**< Buffer to store generated file system path using snprintf */

/**
 * It takes ADC channel number as the input and reads its value using file system access and then returns its value.
 * @param channel an unsigned integer argument.
 * @return If successful then value of ADC channel is returned and if it fails then -1 is returned.
 */

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