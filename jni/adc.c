/**********************************************************
  ADC general purpose interface code for for raw voltage input 
  from file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include <stdint.h>
#include "packtHAL.h"

#define SYSFS_ADC_DIR "/sys/bus/iio/devices/iio:device0/"

static char fsBuf[100];


uint32_t readADC(unsigned int channel)
{
	FILE *fd;
	char *value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_ADC_DIR "/in_voltage%d_raw", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return 0;
	}

  	fscanf(fd, "%s", value);
  	fclose(fd);

  	return atol(value);
}