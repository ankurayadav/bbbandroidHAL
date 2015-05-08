/**********************************************************
  PacktHAL interface to the Adafruit I2C 32KB FRAM
    breakout board: http://www.adafruit.com/products/1895

  Written by Andrew Henderson (hendersa@icculus.org)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "packtHAL.h"

#define EEPROM_SIZE 32768
#define MAX_PATH 40

static off_t FRAMSize = 0;
static int FRAMOpen = 0;
static int fdFRAM = 0;

int openFRAM(const unsigned int bus, const unsigned int address)
{
  char filename[MAX_PATH];
  struct stat statBuf;

  /* Is there already an FRAM open? */
  if (FRAMOpen) return 1;

  /* Is this a valid I2C bus? */
  if (bus > 2) return 1;

  /* Is this a valid address? */
  if ((address < 0x50) || (address > 0x57))
    return 1;

  /* Construct the filepath to the FRAM */
  snprintf(filename, (MAX_PATH - 1),
    "/sys/bus/i2c/devices/%d-00%x/eeprom", bus, address);

  /* Open up the EEPROM */
  fdFRAM = open(filename, O_RDWR);
  if (fdFRAM == -1)
    return 1;

  /* Find size of EEPROM */
  if(stat(filename, &statBuf))
    FRAMSize = EEPROM_SIZE; 
  else
    FRAMSize = statBuf.st_size;

  /* Done! */
  FRAMOpen = 1;
  return 0;
}

int closeFRAM(void)
{
  /* Is an FRAM currently open? */
  if (!FRAMOpen) return 1;

  /* Close FRAM */
  close(fdFRAM);

  /* Done! */
  FRAMOpen = 0;
  return 0;
}

int writeFRAM(const unsigned int offset, const unsigned int bufferSize,
  const char *buffer)
{
  ssize_t remainingBytes, bytesWritten;

  /* Is an FRAM open? */
  if (!FRAMOpen) return 1;

  /* Is the buffer non-NULL? */
  if (!buffer) return 1;

  /* Is this write outside the boundary of the FRAM? */
  if (FRAMSize <= (int)(offset + bufferSize))
    return 1;

  /* Seek to the proper offset in the FRAM */
  lseek(fdFRAM, offset, SEEK_SET); 

  /* Write the data to the FRAM */
  remainingBytes = bufferSize;
  while(remainingBytes > 0)
  {
    bytesWritten = write(fdFRAM, buffer, remainingBytes);
    if (bytesWritten == -1) return 1;
    remainingBytes -= bytesWritten;
  }

  /* Done! */
  return 0;
}

int readFRAM(const unsigned int offset, const unsigned int bufferSize,
  const char *buffer)
{
  ssize_t remainingBytes, bytesRead;

  /* Is an FRAM open? */
  if (!FRAMOpen) return 1;

  /* Is the buffer non-NULL? */
  if (!buffer) return 1;

  /* Is this read outside the boundary of the FRAM? */
  if (FRAMSize <= (int)(offset + bufferSize))
    return 1;

  /* Seek to the proper offset in the FRAM */
  lseek(fdFRAM, offset, SEEK_SET);

  /* Read the data to the FRAM */
  remainingBytes = bufferSize;
  while(remainingBytes > 0)
  {
    bytesRead = read(fdFRAM, (void *)(buffer + (bufferSize - remainingBytes)), 
      remainingBytes);
    if (bytesRead == -1) return 1;
    remainingBytes -= bytesRead;
  }

  /* Done! */
  return 0;
}

