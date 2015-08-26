/**********************************************************
  PWM general purpose interface code for file system

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file pwm.c
 * @author Ankur Yadav (ankurayadav@gmail.com)
 * @brief PWM general purpose interface code for file system
 */

#include <stdio.h>
#include "bbbandroidHAL.h"

#define SYSFS_PWM_DIR "/sys/class/pwm"	/**< File system path to access PWM */

static char fsBuf[100];	/**< Buffer to store generated file system path using snprintf */

/**
 * It takes channel and period to be assigned in nano seconds 
 * and sets period of that channel to specified value using file system.
 * @param channel a constant unsigned integer argument.
 * @param period_ns a constant unsigned integer argument.
 * @return 0 on success and -1 if it fails.
 */

int pwmSetPeriod(const uint8_t channel, const uint32_t period_ns)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/period_ns", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return -1;
	}

	fprintf(fd, "%d", period_ns);
	fclose(fd);

	return 0;
}

/**
 * It takes channel number as input
 * and returns the value of period of specified channel in nano seconds.
 * @param channel a constant unsigned integer argument.
 * @return value of period on success and -1 if it fails.
 */

int pwmGetPeriod(const uint8_t channel)
{
	FILE *fd;
	int value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/period_ns", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return -1;
	}

  	fscanf(fd, "%d", &value);
  	fclose(fd);

  	return value;
}

/**
 * It takes channel and duty cycle to be assigned in nano seconds
 * and sets duty cycle of that channel to specified value using file system.
 * @param channel a constant unsigned integer argument.
 * @param duration_ns a constant unsigned integer argument.
 * @return 0 on success and -1 if it fails.
 */

int pwmSetDutyCycle(const uint8_t channel, const uint32_t duration_ns)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/duty_ns", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return -1;
	}

	fprintf(fd, "%d", duration_ns);
	fclose(fd);

	return 0;
}

/**
 * It takes channel number as input and
 * returns the value of duty cycle of specified channel in nano seconds.
 * @param channel a constant unsigned integer argument.
 * @return value of duty cycle on success and -1 if it fails.
 */

int pwmGetDutyCycle(const uint8_t channel)
{
	FILE *fd;
	int value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/duty_ns", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return -1;
	}

  	fscanf(fd, "%d", &value);
  	fclose(fd);

  	return value;
}

/**
 * It takes channel and polarity to be assigned
 * and sets polarity of that channel to the specified value using file system.
 * @param channel a constant unsigned integer argument.
 * @param polarity a constant unsigned integer argument.
 * @return 0 on success and -1 if it fails.
 */

int pwmSetPolarity(const uint8_t channel, const uint8_t polarity)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/polarity", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return -1;
	}

	fprintf(fd, "%d", polarity);
	fclose(fd);

	return 0;
}

/**
 * It takes channel number as input and 
 * returns the value of polarity of specified channel.
 * @param channel a constant unsigned integer argument.
 * @return value of polarity on success and -1 if it fails.
 */

int pwmGetPolarity(const uint8_t channel)
{
	FILE *fd;
	int value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/polarity", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return -1;
	}

  	fscanf(fd, "%d", &value);
  	fclose(fd);

  	return value;
}

/**
 * It takes channel number as input and sets run 
 * for that channel so that pwm starts running for that channel.
 * @param channel a constant unsigned integer argument.
 * @return 0 on success and -1 if it fails.
 */

int pwmRun(const uint8_t channel)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return -1;
	}

	fprintf(fd, "%d", 1);
	fclose(fd);

	return 0;	
}

/**
 * It takes channel number as input and sets run 
 * to 0 for that channel so that pwm stops for that channel.
 * @param channel a constant unsigned integer argument.
 * @return 0 on success and -1 if it fails.
 */

int pwmStop(const uint8_t channel)
{
	FILE *fd;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "w");

	if (fd == NULL) 
	{
		return -1;
	}

	fprintf(fd, "%d", 0);
	fclose(fd);

	return 0;	
}

/**
 * It takes channel number as input and 
 * returns the value of run for that channel.
 * @param channel a constant unsigned integer argument.
 * @return value of run on success and -1 if it fails.
 */

int pwmRunCheck(const uint8_t channel)
{
	FILE *fd;
	int value;
	
	snprintf(fsBuf, sizeof(fsBuf), SYSFS_PWM_DIR "/pwm%d/run", channel);

	fd = fopen(fsBuf, "r");

	if (fd == NULL) 
	{
		return -1;
	}

  	fscanf(fd, "%d", &value);
  	fclose(fd);

  	return value;
}