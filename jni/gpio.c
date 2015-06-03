/**********************************************************
  GPIO general purpose interface code for both file system
    and mmap() control of GPIOs

  Written by Andrew Henderson (hendersa@icculus.org)

  Modified by Ankur Yadav (ankurayadav@gmail.com)

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

/*************Here Declare Your Pinmux Configuration*******************/
/*******Example for declaring P8_03 AS GPIO DO*******
#define P8_03_GPIO
****************************************************/



/****************************************************************/

static char fsBuf[64];

typedef struct {
  unsigned int id;    /* FS: ID is the file for the pin */
  unsigned char bank; /* MMAP: GPIO bank determines register */
  unsigned int mask;  /* MMAP: Mask determines bit in register */
} GPIOBit_t;

static GPIOBit_t P8_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       	/* P8_01, GND */
 {  0, 0, 0},       	/* P8_02, GND */
 
 #ifdef P8_03_GPIO
	{ 38, 1, 1 << 6}	/* P8_03, GPIO1[6] */
 #else
 	{  0, 0, 0},		/* P8_03, eMMC */
 #endif

 #ifdef P8_04_GPIO
	{ 39, 1, 1 << 7}	/* P8_04, GPIO1[7] */
 #else
 	{  0, 0, 0},		/* P8_04, eMMC */
 #endif

 #ifdef P8_05_GPIO
	{ 34, 1, 1 << 2}	/* P8_05, GPIO1[2] */
 #else
 	{  0, 0, 0},		/* P8_05, eMMC */
 #endif

 #ifdef P8_06_GPIO
	{ 35, 1, 1 << 3}	/* P8_06, GPIO1[3] */
 #else
 	{  0, 0, 0},		/* P8_06, eMMC */
 #endif

 #ifdef P8_07_GPIO
 	{ 66, 2, 1 << 2},   /* P8_07, GPIO2[2] */
 #else
 	{  0, 0, 0},		/* P8_07,  */
 #endif

 #ifdef P8_08_GPIO
 	{ 67, 2, 1 << 3},	/* P8_08, GPIO2[3] */
 #else
 	{  0, 0, 0},		/* P8_08,  */
 #endif

 #ifdef P8_09_GPIO
 	{ 69, 2, 1 << 5},	/* P8_09, GPIO2[5] */
 #else
 	{  0, 0, 0},		/* P8_09,  */
 #endif

 #ifdef P8_10_GPIO
 	{ 68, 2, 1 << 4},	/* P8_10, GPIO2[4] */
 #else
 	{  0, 0, 0},		/* P8_10,  */
 #endif

 #ifdef P8_11_GPIO
 	{ 45, 1, 1 << 13},	/* P8_11, GPIO1[13] */
 #else
 	{  0, 0, 0},		/* P8_11,  */
 #endif

 #ifdef P8_12_GPIO
 	{ 44, 1, 1 << 12},	/* P8_12, GPIO1[12] */
 #else
 	{  0, 0, 0},		/* P8_12,  */
 #endif

 #ifdef P8_13_GPIO
 	{ 23, 0, 1 << 23},	/* P8_13, GPIO0[23] */
 #else
 	{  0, 0, 0},		/* P8_13,  */
 #endif

 #ifdef P8_14_GPIO
 	{ 26, 0, 1 << 26},	/* P8_14, GPIO0[26] */
 #else
 	{  0, 0, 0},		/* P8_14,  */
 #endif

 #ifdef P8_15_GPIO
 	{ 47, 1, 1 << 15},	/* P8_15, GPIO1[15] */
 #else
 	{  0, 0, 0},		/* P8_15,  */
 #endif

 #ifdef P8_16_GPIO
 	{ 46, 1, 1 << 14},	/* P8_16, GPIO1[14] */
 #else
 	{  0, 0, 0},		/* P8_16,  */
 #endif

 #ifdef P8_17_GPIO
 	{ 27, 0, 1 << 27},	/* P8_17, GPIO0[27] */
 #else
 	{  0, 0, 0},		/* P8_17,  */
 #endif

 #ifdef P8_18_GPIO
 	{ 65, 2, 1 << 1}, 	/* P8_18, GPIO2[1] */
 #else
 	{  0, 0, 0},		/* P8_18,  */
 #endif

 #ifdef P8_19_GPIO
 	{ 22, 0, 1 << 22},	/* P8_19, GPIO0[22] */
 #else
 	{  0, 0, 0},		/* P8_19,  */
 #endif

 #ifdef P8_19_GPIO
 	{ 22, 0, 1 << 22},	/* P8_19, GPIO0[22] */
 #else
 	{  0, 0, 0},		/* P8_19,  */
 #endif

 #ifdef P8_20_GPIO
 	{ 63, 1, 1 << 31},	/* P8_20, GPIO1[31] */
 #else
 	{  0, 0, 0},		/* P8_20, eMMC */
 #endif

 #ifdef P8_21_GPIO
 	{ 62, 1, 1 << 30},	/* P8_21, GPIO1[30] */
 #else
 	{  0, 0, 0},		/* P8_21, eMMC */
 #endif

 #ifdef P8_22_GPIO
 	{ 37, 1, 1 << 5},	/* P8_22, GPIO1[5] */
 #else
 	{  0, 0, 0},		/* P8_22, eMMC */
 #endif

 #ifdef P8_23_GPIO
 	{ 36, 1, 1 << 4},	/* P8_23, GPIO1[4] */
 #else
 	{  0, 0, 0},		/* P8_23, eMMC */
 #endif

 #ifdef P8_24_GPIO
 	{ 33, 1, 1 << 1},	/* P8_24, GPIO1[1] */
 #else
 	{  0, 0, 0},		/* P8_24, eMMC */
 #endif

 #ifdef P8_25_GPIO
 	{ 32, 1, 1 << 0},	/* P8_25, GPIO1[0] */
 #else
 	{  0, 0, 0},		/* P8_25, eMMC */
 #endif

 #ifdef P8_26_GPIO
 	{ 61, 1, 1 << 29},	/* P8_26, GPIO1[29] */
 #else
 	{  0, 0, 0},		/* P8_26,  */
 #endif

 #ifdef P8_27_GPIO
 	{ 86, 2, 1 << 22},	/* P8_27, GPIO2[22] */
 #else
 	{  0, 0, 0},		/* P8_27, HDMI */
 #endif

 #ifdef P8_28_GPIO
 	{ 88, 2, 1 << 24},	/* P8_28, GPIO2[24] */
 #else
 	{  0, 0, 0},		/* P8_28, HDMI */
 #endif

 #ifdef P8_29_GPIO
 	{ 87, 2, 1 << 23},	/* P8_29, GPIO2[23] */
 #else
 	{  0, 0, 0},		/* P8_29, HDMI */
 #endif
 
 #ifdef P8_30_GPIO
 	{ 89, 2, 1 << 25},	/* P8_30, GPIO2[25] */
 #else
 	{  0, 0, 0},		/* P8_30, HDMI */
 #endif

 #ifdef P8_31_GPIO
 	{ 10, 0, 1 << 10},	/* P8_31, GPIO0[10] */
 #else
 	{  0, 0, 0},		/* P8_31, HDMI */
 #endif

 #ifdef P8_32_GPIO
 	{ 11, 0, 1 << 11},	/* P8_32, GPIO0[11] */
 #else
 	{  0, 0, 0},		/* P8_32, HDMI */
 #endif

 #ifdef P8_33_GPIO
 	{ 9, 0, 1 << 9},	/* P8_33, GPIO0[9] */
 #else
 	{  0, 0, 0},		/* P8_33, HDMI */
 #endif

 #ifdef P8_34_GPIO
 	{ 81, 2, 1 << 17},	/* P8_34, GPIO2[17] */
 #else
 	{  0, 0, 0},		/* P8_34, HDMI */
 #endif

 #ifdef P8_35_GPIO
 	{ 8, 0, 1 << 8},	/* P8_35, GPIO0[8] */
 #else
 	{  0, 0, 0},		/* P8_35, HDMI */
 #endif

 #ifdef P8_36_GPIO
 	{ 80, 2, 1 << 16},	/* P8_36, GPIO2[16] */
 #else
 	{  0, 0, 0},		/* P8_36, HDMI */
 #endif

 #ifdef P8_37_GPIO
 	{ 78, 2, 1 << 14},	/* P8_37, GPIO2[14] */
 #else
 	{  0, 0, 0},		/* P8_37, HDMI */
 #endif

 #ifdef P8_38_GPIO
 	{ 79, 2, 1 << 15},	/* P8_38, GPIO2[15] */
 #else
 	{  0, 0, 0},		/* P8_38, HDMI */
 #endif

 #ifdef P8_39_GPIO
 	{ 76, 2, 1 << 12},	/* P8_39, GPIO2[12] */
 #else
 	{  0, 0, 0},		/* P8_39, HDMI */
 #endif

 #ifdef P8_40_GPIO
 	{ 77, 2, 1 << 13},	/* P8_40, GPIO2[13] */
 #else
 	{  0, 0, 0},		/* P8_40, HDMI */
 #endif

 #ifdef P8_41_GPIO
 	{ 74, 2, 1 << 10},	/* P8_41, GPIO2[10] */
 #else
 	{  0, 0, 0},		/* P8_41, HDMI */
 #endif

 #ifdef P8_42_GPIO
 	{ 75, 2, 1 << 11},	/* P8_42, GPIO2[11] */
 #else
 	{  0, 0, 0},		/* P8_42, HDMI */
 #endif

 #ifdef P8_43_GPIO
 	{ 72, 2, 1 << 8},	/* P8_43, GPIO2[8] */
 #else
 	{  0, 0, 0},		/* P8_43, HDMI */
 #endif

 #ifdef P8_44_GPIO
 	{ 73, 2, 1 << 9},	/* P8_44, GPIO2[9] */
 #else
 	{  0, 0, 0},		/* P8_44, HDMI */
 #endif

 #ifdef P8_45_GPIO
 	{ 70, 2, 1 << 6},	/* P8_45, GPIO2[6] */
 #else
 	{  0, 0, 0},		/* P8_45, HDMI */
 #endif

 #ifdef P8_46_GPIO
 	{ 71, 2, 1 << 7},	/* P8_46, GPIO2[7] */
 #else
 	{  0, 0, 0},		/* P8_46, HDMI */
 #endif

};

static GPIOBit_t P9_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       	/* P9_01, Power/Control */
 {  0, 0, 0},       	/* P9_02, Power/Control */
 {  0, 0, 0},       	/* P9_03, Power/Control */
 {  0, 0, 0},       	/* P9_04, Power/Control */
 {  0, 0, 0},       	/* P9_05, Power/Control */
 {  0, 0, 0},       	/* P9_06, Power/Control */
 {  0, 0, 0},       	/* P9_07, Power/Control */
 {  0, 0, 0},       	/* P9_08, Power/Control */
 {  0, 0, 0},       	/* P9_09, Power/Control */
 {  0, 0, 0},       	/* P9_10, Power/Control */

 #ifdef P9_11_GPIO
 	{ 30, 0, 1 << 30},	/* P9_11, GPIO0[30] */
 #else
 	{  0, 0, 0},		/* P9_11,  */
 #endif

 #ifdef P9_12_GPIO
 	{ 60, 1, 1 << 28},	/* P9_12, GPIO1[28] */
 #else
 	{  0, 0, 0},		/* P9_12,  */
 #endif

 #ifdef P9_13_GPIO
 	{ 31, 0, 1 << 31},	/* P9_13, GPIO0[31] */
 #else
 	{  0, 0, 0},		/* P9_13,  */
 #endif

 #ifdef P9_14_GPIO
 	{ 50, 1, 1 << 18},	/* P9_14, GPIO1[18] */
 #else
 	{  0, 0, 0},		/* P9_14,  */
 #endif

 #ifdef P9_15_GPIO
 	{ 48, 1, 1 << 16},	/* P9_15, GPIO1[16] */
 #else
 	{  0, 0, 0},		/* P9_15,  */
 #endif

 #ifdef P9_16_GPIO
 	{ 51, 1, 1 << 19},	/* P9_16, GPIO1[19] */
 #else
 	{  0, 0, 0},		/* P9_16,  */
 #endif

 #ifdef P9_17_GPIO
 	{ 5, 0, 1 << 5}, 	/* P9_17, GPIO0[5] */
 #else
 	{  0, 0, 0},		/* P9_17,  */
 #endif

 #ifdef P9_18_GPIO
 	{ 4, 0, 1 << 4}, 	/* P9_18, GPIO0[4] */
 #else
 	{  0, 0, 0},		/* P9_18,  */
 #endif

 #ifdef P9_19_GPIO
 	{ 13, 0, 1 << 13},	/* P9_19, GPIO0[13] */
 #else
 	{  0, 0, 0},		/* P9_19, Capebus */
 #endif

 #ifdef P9_20_GPIO
 	{ 12, 0, 1 << 12}, 	/* P9_20, GPIO0[12] */
 #else
 	{  0, 0, 0},		/* P9_20, Capebus */
 #endif

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

