/**********************************************************
  GPIO general purpose interface code for both file system
    and mmap() control of GPIOs

  Written by Andrew Henderson (hendersa@icculus.org)

  Modified by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file gpio.c
 * @author Ankur Yadav (ankurayadav@gmail.com), Andrew Henderson (hendersa@icculus.org)
 * @brief GPIO general purpose interface code for both file system and mmap() control of GPIOs
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include "bbbandroidHAL.h"

#define TOTAL_PINS_PER_HEADER 46		/**< Total number of pins per header in beaglebone black */
#define GPIO_DATA_OUT_REG     0x13C		/**< GPIO DATA OUT register address */
#define GPIO_DATA_IN_REG      0x138		/**< GPIO DATA IN register address */
#define GPIO_OE_REG           0x134		/**< GPIO OE register address */
#define SYSFS_GPIO_DIR        "/sys/class/gpio"		/**< File system path to access GPIO */

static char fsBuf[64];  /**< Buffer to store generated file system path using snprintf */

/** 
 * typedef struct GPIOBit_t for storing parameters to access GPIOs using Memory Map.
 * 
 */

typedef struct {
  unsigned int id;    /**< FS: ID is the file for the pin */
  unsigned char bank; /**< MMAP: GPIO bank determines register */
  unsigned int mask;  /**< MMAP: Mask determines bit in register */
} GPIOBit_t;

/** 
 * static GPIOBit_t variable to store P8 header pins.
 */

static GPIOBit_t P8_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       /* P8_01, GND */
 {  0, 0, 0},       /* P8_02, GND */
 { 38, 1, 1 << 6},	/* P8_03, GPIO1[6] */
 { 39, 1, 1 << 7},	/* P8_04, GPIO1[7] */
 { 34, 1, 1 << 2},	/* P8_05, GPIO1[2] */
 { 35, 1, 1 << 3},	/* P8_06, GPIO1[3] */
 { 66, 2, 1 << 2},  /* P8_07, GPIO2[2] */
 { 67, 2, 1 << 3},	/* P8_08, GPIO2[3] */
 { 69, 2, 1 << 5},	/* P8_09, GPIO2[5] */
 { 68, 2, 1 << 4},	/* P8_10, GPIO2[4] */
 { 45, 1, 1 << 13},	/* P8_11, GPIO1[13] */
 { 44, 1, 1 << 12},	/* P8_12, GPIO1[12] */
 { 23, 0, 1 << 23},	/* P8_13, GPIO0[23] */
 { 26, 0, 1 << 26},	/* P8_14, GPIO0[26] */
 { 47, 1, 1 << 15},	/* P8_15, GPIO1[15] */
 { 46, 1, 1 << 14},	/* P8_16, GPIO1[14] */
 { 27, 0, 1 << 27},	/* P8_17, GPIO0[27] */
 { 65, 2, 1 << 1}, 	/* P8_18, GPIO2[1] */
 { 22, 0, 1 << 22},	/* P8_19, GPIO0[22] */
 { 63, 1, 1 << 31},	/* P8_20, GPIO1[31] */
 { 62, 1, 1 << 30},	/* P8_21, GPIO1[30] */
 { 37, 1, 1 << 5},	/* P8_22, GPIO1[5] */
 { 36, 1, 1 << 4},	/* P8_23, GPIO1[4] */
 { 33, 1, 1 << 1},	/* P8_24, GPIO1[1] */
 { 32, 1, 1 << 0},	/* P8_25, GPIO1[0] */
 { 61, 1, 1 << 29},	/* P8_26, GPIO1[29] */
 { 86, 2, 1 << 22},	/* P8_27, GPIO2[22] */
 { 88, 2, 1 << 24},	/* P8_28, GPIO2[24] */
 { 87, 2, 1 << 23},	/* P8_29, GPIO2[23] */
 { 89, 2, 1 << 25},	/* P8_30, GPIO2[25] */
 { 10, 0, 1 << 10},	/* P8_31, GPIO0[10] */
 { 11, 0, 1 << 11},	/* P8_32, GPIO0[11] */
 { 9, 0, 1 << 9},	/* P8_33, GPIO0[9] */
 { 81, 2, 1 << 17},	/* P8_34, GPIO2[17] */
 { 8, 0, 1 << 8},	/* P8_35, GPIO0[8] */
 { 80, 2, 1 << 16},	/* P8_36, GPIO2[16] */
 { 78, 2, 1 << 14},	/* P8_37, GPIO2[14] */
 { 79, 2, 1 << 15},	/* P8_38, GPIO2[15] */
 { 76, 2, 1 << 12},	/* P8_39, GPIO2[12] */
 { 77, 2, 1 << 13},	/* P8_40, GPIO2[13] */
 { 74, 2, 1 << 10},	/* P8_41, GPIO2[10] */
 { 75, 2, 1 << 11},	/* P8_42, GPIO2[11] */
 { 72, 2, 1 << 8},	/* P8_43, GPIO2[8] */
 { 73, 2, 1 << 9},	/* P8_44, GPIO2[9] */
 { 70, 2, 1 << 6},	/* P8_45, GPIO2[6] */
 { 71, 2, 1 << 7}	/* P8_46, GPIO2[7] */
};

/** 
 * static GPIOBit_t variable to store P9 header pins.
 */

static GPIOBit_t P9_GPIO_pin_info[TOTAL_PINS_PER_HEADER] = {
 {  0, 0, 0},       /* P9_01, Power/Control */
 {  0, 0, 0},       /* P9_02, Power/Control */
 {  0, 0, 0},       /* P9_03, Power/Control */
 {  0, 0, 0},       /* P9_04, Power/Control */
 {  0, 0, 0},       /* P9_05, Power/Control */
 {  0, 0, 0},       /* P9_06, Power/Control */
 {  0, 0, 0},      	/* P9_07, Power/Control */
 {  0, 0, 0},      	/* P9_08, Power/Control */
 {  0, 0, 0},      	/* P9_09, Power/Control */
 {  0, 0, 0},      	/* P9_10, Power/Control */
 { 30, 0, 1 << 30},	/* P9_11, GPIO0[30] */
 { 60, 1, 1 << 28},	/* P9_12, GPIO1[28] */
 { 31, 0, 1 << 31},	/* P9_13, GPIO0[31] */
 { 50, 1, 1 << 18},	/* P9_14, GPIO1[18] */
 { 48, 1, 1 << 16},	/* P9_15, GPIO1[16] */
 { 51, 1, 1 << 19},	/* P9_16, GPIO1[19] */
 {  5, 0, 1 << 5}, 	/* P9_17, GPIO0[5] */
 {  4, 0, 1 << 4}, 	/* P9_18, GPIO0[4] */
 { 13, 0, 1 << 13},	/* P9_19, GPIO0[13] */
 { 12, 0, 1 << 12}, /* P9_20, GPIO0[12] */
 {  3, 0, 1 << 3}, 	/* P9_21, GPIO0[3] */
 {  2, 0, 1 << 2}, 	/* P9_22, GPIO0[2] */
 { 49, 1, 1 << 17}, /* P9_23, GPIO1[17] */
 { 15, 0, 1 << 15}, /* P9_24, GPIO0[15] */
 {117, 3, 1 << 21},	/* P9_25, GPIO3[21] */
 { 14, 0, 1 << 14}, /* P9_26, GPIO0[14] */
 {115, 3, 1 << 19}, /* P9_27, GPIO3[19] */
 {113, 3, 1 << 17}, /* P9_28, GPIO3[17] */
 {111, 3, 1 << 15}, /* P9_29, GPIO3[15] */ 
 {112, 3, 1 << 16}, /* P9_30, GPIO3[16] */
 {110, 3, 1 << 14}, /* P9_31, GPIO3[14] */
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
  { 0x44E07000, 0x4804C000, 0x481AC000, 0x481AE000 };	/**< Register Bank addresses */
static uint32_t *mapGPIO[4];	/**< Variable for GPIO memory map */
static int fdGPIO = 0;			/**< File descriptor for GPIO for file system access */
static int initialized = 0;		/**< Variable to check if GPIO is initialized earlier */
static int usingMmap = 0;		/**< Variable to check if Memory map access mode for GPIO is set */

/**
 * This function takes parameter input useMmap
 * to take choice if you want to use Memory Map to access GPIO or to access it using 
 * file system. If using memory map then a proper file descriptor is set.
 * @param useMmap a constant integer argument.
 * @return If successful then 0 is returned and if it fails then 1 is returned.
 */

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

/**
 * This function takes input pointer to GPIOBit_t type structure which stores information about a pin.
 * This pin information is then used to get DATA IN register and read the GPIO.
 * @param *pinGPIO a constant GPIOBit_t argument.
 * @return The bit for the GPIO that we're interested in.
 */

static int readGPIOMmap(const GPIOBit_t *pinGPIO) 
{
  unsigned int reg;

  /* Get the appropriate DATA IN register */
  reg = mapGPIO[pinGPIO->bank][GPIO_DATA_IN_REG/4];

  /* Return the bit for the GPIO that we're interested in */
  return (reg & pinGPIO->mask);
}

/**
 * This function takes input pointer to GPIOBit_t type structure which stores information about a pin.
 * This pin information is then used to get DATA OUT register and OE register to write to GPIO.
 * @param *pinGPIO a constant GPIOBit_t argument.
 * @return 0
 */

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

/**
 * This function takes input pointer to GPIOBit_t type structure which stores information about a pin.
 * This pin information is then used to get pin id to read from GPIO.
 * @param *pinGPIO a constant GPIOBit_t argument.
 * @return 1 if read 1 and 0 if read 0;
 */

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

/**
 * This function takes input pointer to GPIOBit_t type structure which stores information about a pin.
 * This pin information is then used to get pin id to write to GPIO.
 * @param *pinGPIO a constant GPIOBit_t argument.
 * @return 0 if successfull and 1 if it fails
 */

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

/**
 * It takes input GPIO header and pin and reads its value after opening its file.
 * If Memory Map is used then proper bit value is read using the file descriptor initialized in openGPIO() function call.
 * Proper bit positions for all the GPIOs are present in GPIOBit_t arrays of each P8 and P9 header.
 * @param header a constant unsigned int argument.
 * @param pin a constant unsigned int argument.
 * @see readGPIOMmap()
 * @see readGPIOFS()
 * @return 1 if read 1 and 0 if read 0;
 */

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

/**
 * It takes input GPIO header, pin and value that is required to be set on GPIO after opening corresponding GPIO file.
 * If Memory Map is used then proper bit value is set using the file descriptor initialized in openGPIO() function call.
 * Proper bit positions for all the GPIOs are present in GPIOBit_t arrays of each P8 and P9 header.
 * @param header a constant unsigned int argument.
 * @param pin a constant unsigned int argument.
 * @see writeGPIOMmap()
 * @see writeGPIOFS()
 * @return 0 if successfull and 1 if it fails
 */

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

/**
 * For closing file descriptor if accessed using file system and set initialized to 0.
 */

void closeGPIO(void) {
  if (initialized)
    if (usingMmap)
      close(fdGPIO);
  initialized = 0;
}

