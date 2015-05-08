/**********************************************************
  GPIO general purpose interface code for both file system
    and mmap() control of GPIOs

  Written by Andrew Henderson (hendersa@icculus.org)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include "packtHAL.h"

#define TOTAL_PINS_PER_HEADER 46
#define GPIO_DATA_OUT_REG     0x13C
#define GPIO_DATA_IN_REG      0x138
#define GPIO_OE_REG           0x134
#define SYSFS_GPIO_DIR        "/sys/class/gpio"

static char fsBuf[64];

typedef struct {
  unsigned int id;    /* FS: ID is the file for the pin */
  unsigned char bank; /* MMAP: GPIO bank determines register */
  unsigned int mask;  /* MMAP: Mask determines bit in register */
} GPIOBit_t;

static GPIOBit_t P8_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       /* P8_01, GND */
 {  0, 0, 0},       /* P8_02, GND */
 {  0, 0, 0},       /* P8_03, eMMC */
 {  0, 0, 0},       /* P8_04, eMMC */
 {  0, 0, 0},       /* P8_05, eMMC */
 {  0, 0, 0},       /* P8_06, eMMC */
 { 66, 2, 1 << 2},  /* P8_07, GPIO2[2] */
 { 67, 2, 1 << 3},  /* P8_08, GPIO2[3] */
 { 69, 2, 1 << 5},  /* P8_09, GPIO2[5] */
 { 68, 2, 1 << 4},  /* P8_10, GPIO2[4] */
 { 45, 1, 1 << 13}, /* P8_11, GPIO1[13] */
 { 44, 1, 1 << 12}, /* P8_12, GPIO1[12] */
 { 23, 0, 1 << 23}, /* P8_13, GPIO0[23] */
 { 26, 0, 1 << 26}, /* P8_14, GPIO0[26] */
 { 47, 1, 1 << 15}, /* P8_15, GPIO1[15] */
 { 46, 1, 1 << 14}, /* P8_16, GPIO1[14] */
 { 27, 0, 1 << 27}, /* P8_17, GPIO0[27] */
 { 65, 2, 1 << 1},  /* P8_18, GPIO2[1] */
 { 22, 0, 1 << 22}, /* P8_19, GPIO0[22] */
 {  0, 0, 0},       /* P8_20, eMMC */
 {  0, 0, 0},       /* P8_21, eMMC */
 {  0, 0, 0},       /* P8_22, eMMC */
 {  0, 0, 0},       /* P8_23, eMMC */
 {  0, 0, 0},       /* P8_24, eMMC */
 {  0, 0, 0},       /* P8_25, eMMC */
 { 61, 1, 1 << 29}, /* P8_26, GPIO1[29] */
 {  0, 0, 0},       /* P8_27, HDMI */
 {  0, 0, 0},       /* P8_28, HDMI */
 {  0, 0, 0},       /* P8_29, HDMI */
 {  0, 0, 0},       /* P8_30, HDMI */
 {  0, 0, 0},       /* P8_31, HDMI */
 {  0, 0, 0},       /* P8_32, HDMI */
 {  0, 0, 0},       /* P8_33, HDMI */
 {  0, 0, 0},       /* P8_34, HDMI */
 {  0, 0, 0},       /* P8_35, HDMI */
 {  0, 0, 0},       /* P8_36, HDMI */
 {  0, 0, 0},       /* P8_37, HDMI */
 {  0, 0, 0},       /* P8_38, HDMI */
 {  0, 0, 0},       /* P8_39, HDMI */
 {  0, 0, 0},       /* P8_40, HDMI */
 {  0, 0, 0},       /* P8_41, HDMI */
 {  0, 0, 0},       /* P8_42, HDMI */
 {  0, 0, 0},       /* P8_43, HDMI */
 {  0, 0, 0},       /* P8_44, HDMI */
 {  0, 0, 0},       /* P8_45, HDMI */
 {  0, 0, 0},       /* P8_46, HDMI */
};

static GPIOBit_t P9_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       /* P9_01, Power/Control */
 {  0, 0, 0},       /* P9_02, Power/Control */
 {  0, 0, 0},       /* P9_03, Power/Control */
 {  0, 0, 0},       /* P9_04, Power/Control */
 {  0, 0, 0},       /* P9_05, Power/Control */
 {  0, 0, 0},       /* P9_06, Power/Control */
 {  0, 0, 0},       /* P9_07, Power/Control */
 {  0, 0, 0},       /* P9_08, Power/Control */
 {  0, 0, 0},       /* P9_09, Power/Control */
 {  0, 0, 0},       /* P9_10, Power/Control */
 { 30, 0, 1 << 30}, /* P9_11, GPIO0[30] */
 { 60, 1, 1 << 28}, /* P9_12, GPIO1[28] */
 { 31, 0, 1 << 31}, /* P9_13, GPIO0[31] */
 { 50, 1, 1 << 18}, /* P9_14, GPIO1[18] */
 { 48, 1, 1 << 16}, /* P9_15, GPIO1[16] */
 { 51, 1, 1 << 19}, /* P9_16, GPIO1[19] */
 {  5, 0, 1 << 5},  /* P9_17, GPIO0[5] */
 {  4, 0, 1 << 4},  /* P9_18, GPIO0[4] */
 {  0, 0, 0},       /* P9_19, Capebus */
 {  0, 0 ,0},       /* P9_20, Capebus */
 { 49, 1, 1 << 17}, /* P9_23, GPIO1[17] */
 { 15, 0, 1 << 15}, /* P9_24, GPIO0[15] */
 {  0, 0, 0},       /* P9_25, McASP */
 { 14, 0, 1 << 14}, /* P9_26, GPIO0[14] */
 {115, 3, 1 << 19}, /* P9_27, GPIO3[19] */
 {  0, 0, 0},       /* P9_28, McASP */
 {  0, 0, 0},       /* P9_29, McASP */ 
 {112, 3, 1 << 16}, /* P9_30, GPIO3[16] */
 {  0, 0, 0},       /* P9_31, McASP */
 {  0, 0, 0},       /* P9_32, ADC */
 {  0, 0, 0},       /* P9_33, ADC */
 {  0, 0, 0},       /* P9_34, ADC */
 {  0, 0, 0},       /* P9_35, ADC */
 {  0, 0, 0},       /* P9_36, ADC */
 {  0, 0, 0},       /* P9_37, ADC */
 {  0, 0, 0},       /* P9_38, ADC */
 {  0, 0, 0},       /* P9_39, ADC */
 {  0, 0, 0},       /* P9_40, ADC */
 { 20, 0, 1 << 20}, /* P9_41A, GPIO0[20] */
 {  7, 0, 1 << 7},  /* P9_42A, GPIO0[7] */
 {  0, 0, 0},       /* P9_43, GND */
 {  0, 0, 0},       /* P9_44, GND */
 {  0, 0, 0},       /* P9_45, GND */
 {  0, 0, 0}        /* P9_46, GND */
};

static const uint32_t gpioAddrs[] = 
  { 0x44E07000, 0x4804C000, 0x481AC000, 0x481AE000 };
static uint32_t *mapGPIO[4];
static int fdGPIO = 0;
static int initialized = 0;
static int usingMmap = 0;

int openGPIO(const int useMmap)
{
  int i;

  /* Have we already picked a GPIO access method? */
  if (initialized)
    return 1;

  usingMmap = useMmap;

  /* Are we using mmap() for the GPIO access? */
  if (usingMmap) {
    fdGPIO = open("/dev/mem", O_RDWR | O_SYNC);

    /* mmap() the four GPIO bank registers */
    for (i = 0; i < 4; i++)
    {
      mapGPIO[i] = (uint32_t *) mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fdGPIO, gpioAddrs[i]);
      /*printf("gpio[%i] at address 0x%08x mapped at 0x%08x\n",i,gpioAddrs[i],(unsigned int)mapGPIO[i]);*/
      if (mapGPIO[i] == (uint32_t *)-1) {
        printf("GPIO: errno[%d]: '%s'\n", errno, strerror(errno));
        return 1;
      }
    }
  }

  /* Done! */
  initialized = 1;
  return 0;
}

static int readGPIOMmap(const GPIOBit_t *pinGPIO) 
{
  unsigned int reg;

  /* Get the appropriate DATA IN register */
  reg = mapGPIO[pinGPIO->bank][GPIO_DATA_IN_REG/4];

  /* Return the bit for the GPIO that we're interested in */
  return (reg & pinGPIO->mask);
}

static int writeGPIOMmap(const GPIOBit_t *pinGPIO, 
  const unsigned int value) 
{
  unsigned int reg;

  /* Get the appropriate OE register */
  reg = mapGPIO[pinGPIO->bank][GPIO_DATA_OUT_REG/4];
  if (value) { /* Set the output bit */
    /* Set the output bit */
    reg = reg | pinGPIO->mask;

    /* Set the appropriate OE register */
    mapGPIO[pinGPIO->bank][GPIO_DATA_OUT_REG/4] = reg;
  } else { /* Clear the output bit */
    /* Clear the output bit */
    reg = reg & (0xFFFFFFFF - pinGPIO->mask);

    /* Set the appropriate OE register */
    mapGPIO[pinGPIO->bank][GPIO_DATA_OUT_REG/4] = reg;
  }
  return 0;
}

static int readGPIOFS(const GPIOBit_t *pinGPIO)
{
  int fd, len;
  char ch;

  snprintf(fsBuf, sizeof(fsBuf), SYSFS_GPIO_DIR "/gpio%d/value",
    pinGPIO->id);

  fd = open(fsBuf, O_RDONLY);
  if (fd < 0)
    return 0;
 
  len = read(fd, &ch, 1);
  close(fd);

  /* If we were able to read a "1", return 1 */
  if ((len > 0) && (ch != '0'))
    return 1;

  return 0;
}

static int writeGPIOFS(const GPIOBit_t *pinGPIO, 
  const unsigned int value)
{
  int fd, len;

  snprintf(fsBuf, sizeof(fsBuf), SYSFS_GPIO_DIR "/gpio%d/value", 
    pinGPIO->id);

  fd = open(fsBuf, O_WRONLY);
  if (fd < 0) {
    return 1;
  }

  if (value)
    len = write(fd, "1", 2);
  else
    len = write(fd, "0", 2);

  if (len < 2)
    return 1;

  close(fd);
  return 0;
}

int readGPIO(const unsigned int header, const unsigned int pin) 
{
  GPIOBit_t *pinGPIO;
 
  if (initialized)
  {
    /* If an invalid pin was specified, return 0 */
    if ((pin == 0) || (pin > TOTAL_PINS_PER_HEADER))
      return 0;

    /* Find the entry for this pin */
    if (header == 8)
      pinGPIO = &(P8_GPIO_pin_info[pin - 1]);
    else if (header == 9)
      pinGPIO = &(P9_GPIO_pin_info[pin - 1]);
    else
      return 0; /* If an invalid header was specified, return 0 */

    /* Is this pin not a GPIO? */
    if (!pinGPIO->mask) return 0;

    if (usingMmap)
      return readGPIOMmap(pinGPIO);
    else
      return readGPIOFS(pinGPIO);
  } 
  else
    return 0;
}

int writeGPIO(const unsigned int header, const unsigned int pin,
  const unsigned int value) 
{
  GPIOBit_t *pinGPIO;

  if (initialized)
  {
    /* If an invalid pin was specified, return 1 */
    if ((pin == 0) || (pin > TOTAL_PINS_PER_HEADER))
      return 0;

    /* Find the entry for this pin */
    if (header == 8)
      pinGPIO = &(P8_GPIO_pin_info[pin - 1]);
    else if (header == 9)
      pinGPIO = &(P9_GPIO_pin_info[pin - 1]);
    else
      return 1; /* If an invalid header was specified, return 1 */

    /* Is this pin not a GPIO? */
    if (!pinGPIO->mask) return 1;

    if (usingMmap)
      return writeGPIOMmap(pinGPIO, value);
    else
      return writeGPIOFS(pinGPIO, value);
  } 
  else
    return 0;
}

void closeGPIO(void) {
  if (initialized)
    if (usingMmap)
      close(fdGPIO);
  initialized = 0;
}

