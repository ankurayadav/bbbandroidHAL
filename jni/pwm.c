/**********************************************************
  PWM general purpose interface code for for raw voltage input 
  from file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include "packtHAL.h"

#define SYSFS_PWM_DIR "/sys/class/pwm"

static char fsBuf[100];

int pwmSetPeriod(unsigned int channel, unsigned int period_ns)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/period_ns", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return 0;
	}

	fprintf(fd, "%d", period_ns);
	fclose(fd);

	return 1;
}

unsigned int pwmGetPeriod(unsigned int channel)
{
	FILE *fd;
	char *value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/period_ns", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return 0;
	}

  	fscanf(fd, "%s", value);
  	fclose(fd);

  	return atol(value);
}

int pwmSetDutyCycle(unsigned int channel, unsigned int duration_ns)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/duty_ns", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return 0;
	}

	fprintf(fd, "%d", duration_ns);
	fclose(fd);

	return 1;
}

int pwmGetDutyCycle(unsigned int channel)
{
	FILE *fd;
	char *value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/duty_ns", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return 0;
	}

  	fscanf(fd, "%s", value);
  	fclose(fd);

  	return atol(value);
}

int pwmSetPolarity(unsigned int channel, unsigned int polarity)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/polarity", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return 0;
	}

	fprintf(fd, "%d", polarity);
	fclose(fd);

	return 1;
}

int pwmGetPolarity(unsigned int channel)
{
	FILE *fd;
	char *value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/polarity", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return 0;
	}

  	fscanf(fd, "%s", value);
  	fclose(fd);

  	return atol(value);
}

int pwmRun(unsigned int channel)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return 0;
	}

	fprintf(fd, "%d", 1);
	fclose(fd);

	return 1;	
}

int pwmStop(unsigned int channel)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return 0;
	}

	fprintf(fd, "%d", 0);
	fclose(fd);

	return 1;	
}

int pwmRunCheck(unsigned int channel)
{
	FILE *fd;
	char *value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return 0;
	}

  	fscanf(fd, "%s", value);
  	fclose(fd);

  	return atol(value);
}