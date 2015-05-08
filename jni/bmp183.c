/**********************************************************
  PacktHAL interface to the Adafruit BMP183 pressure sensor
    breakout board: http://www.adafruit.com/products/1900

  Written by Andrew Henderson (hendersa@icculus.org)

  Based on the Adafruit Industries code originally 
    written by Kevin Townsend

  This code is made available under the BSD license.
**********************************************************/
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <math.h>
#include "packtHAL.h"

#define BUF_SIZE 32
#define EEPROM_SIZE 32768
#define MAX_PATH 40

static int SPIOpen = 0;
static int fdSPI = 0;
static int speedHz = 16000000; /* 16 MHz */
static int mode = 0;
static int bitsPerWord = 8;
static int delay = 0;
static uint8_t txBuffer[16], rxBuffer[16];

/* BMP183 Registers */
enum
{
  BMP183_REGISTER_CAL_AC1         = 0xAA,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_AC2         = 0xAC,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_AC3         = 0xAE,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_AC4         = 0xB0,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_AC5         = 0xB2,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_AC6         = 0xB4,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_B1          = 0xB6,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_B2          = 0xB8,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_MB          = 0xBA,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_MC          = 0xBC,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CAL_MD          = 0xBE,  // R   Calibration data (16 bits)
  BMP183_REGISTER_CHIPID          = 0xD0,
  BMP183_REGISTER_VERSION         = 0xD1,
  BMP183_REGISTER_SOFTRESET       = 0xE0,
  BMP183_REGISTER_CONTROL         = 0xF4,
  BMP183_REGISTER_TEMPDATA        = 0xF6,
  BMP183_REGISTER_PRESSUREDATA    = 0xF6,
  BMP183_REGISTER_READTEMPCMD     = 0x2E,
  BMP183_REGISTER_READPRESSURECMD = 0x34
};

/* BMP183 Modes */
typedef enum
{
  BMP183_MODE_ULTRALOWPOWER       = 0,
  BMP183_MODE_STANDARD            = 1,
  BMP183_MODE_HIGHRES             = 2,
  BMP183_MODE_ULTRAHIGHRES        = 3
} bmp183_mode_t;

/* BMP183 Calibration */
typedef struct
{
  int16_t  ac1;
  int16_t  ac2;
  int16_t  ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t  b1;
  int16_t  b2;
  int16_t  mb;
  int16_t  mc;
  int16_t  md;
} bmp183_calib_data;

static bmp183_calib_data coeffs;
static uint8_t _bmp183Mode = BMP183_MODE_STANDARD;

static int SPIDataTxRx(const int txSize, uint8_t *txBuffer, uint8_t *rxBuffer)
{

  int ret;
  struct spi_ioc_transfer trans = {
    .tx_buf = (unsigned long)txBuffer,
    .rx_buf = (unsigned long)rxBuffer,
    .len = txSize,
    .delay_usecs = delay,
    .speed_hz = speedHz,
    .bits_per_word = bitsPerWord,
  };

  ret = ioctl(fdSPI, SPI_IOC_MESSAGE(1), &trans);
  if (ret < 1)
  {
    fprintf(stderr, "ERROR: SPIDataTxRx() -> SPI transfer failed\n");
    return(1);
  }

  return(0);
}

static int writeCommand(uint8_t reg, uint8_t value)
{
  txBuffer[0] = (reg & 0x7F);
  txBuffer[1] = value;
  return SPIDataTxRx(5, txBuffer, rxBuffer);
}

static uint8_t read8(uint8_t reg)
{
  rxBuffer[0] = 0;
  txBuffer[0] = (reg | 0x80);
  SPIDataTxRx(2, txBuffer, rxBuffer);
  return rxBuffer[1];
}

static uint16_t read16(uint8_t reg)
{
  rxBuffer[1] = rxBuffer[2] = 0;
  txBuffer[0] = (reg | 0x80);
  txBuffer[1] = txBuffer[2] = 0;
  SPIDataTxRx(3, txBuffer, rxBuffer);
  return ((rxBuffer[1] << 8) + rxBuffer[2]);
}

static int16_t readS16(uint8_t reg)
{
  return (int16_t)read16(reg);
}

static void readCoefficients(void)
{
  coeffs.ac1 = readS16(BMP183_REGISTER_CAL_AC1);
  coeffs.ac2 = readS16(BMP183_REGISTER_CAL_AC2);
  coeffs.ac3 = readS16(BMP183_REGISTER_CAL_AC3);
  coeffs.ac4 = read16(BMP183_REGISTER_CAL_AC4);
  coeffs.ac5 = read16(BMP183_REGISTER_CAL_AC5);
  coeffs.ac6 = read16(BMP183_REGISTER_CAL_AC6);
  coeffs.b1 = readS16(BMP183_REGISTER_CAL_B1);
  coeffs.b2 = readS16(BMP183_REGISTER_CAL_B2);
  coeffs.mb = readS16(BMP183_REGISTER_CAL_MB);
  coeffs.mc = readS16(BMP183_REGISTER_CAL_MC);
  coeffs.md = readS16(BMP183_REGISTER_CAL_MD);
}

static int16_t readRawTemperature(void)
{
  writeCommand(BMP183_REGISTER_CONTROL, BMP183_REGISTER_READTEMPCMD);
  usleep(5000);
  return  read16(BMP183_REGISTER_TEMPDATA);
}

static int32_t readRawPressure(void)
{
  uint8_t  p8;
  uint16_t p16;
  int32_t  p32;

  writeCommand(BMP183_REGISTER_CONTROL, BMP183_REGISTER_READPRESSURECMD + (_bmp183Mode << 6));
  switch(_bmp183Mode)
  {
    case BMP183_MODE_ULTRALOWPOWER:
      usleep(5000);
      break;
    case BMP183_MODE_STANDARD:
      usleep(8000);
      break;
    case BMP183_MODE_HIGHRES:
      usleep(14000);
      break;
    case BMP183_MODE_ULTRAHIGHRES:
    default:
      usleep(26000);
      break;
  }

  p16 = read16(BMP183_REGISTER_PRESSUREDATA);
  p32 = (uint32_t)p16 << 8;
  p8 = read8(BMP183_REGISTER_PRESSUREDATA+2);
  p32 += p8;
  p32 >>= (8 - _bmp183Mode);

  return p32;
}

int openSensor(void)
{
  int ret;
  uint8_t id;

  /* Is the SPI bus already open? */
  if (SPIOpen) return 1;

  /* Open up the SPI bus */
  fdSPI = open("/dev/spidev1.0", O_RDWR);
  if (fdSPI == -1)
    return 1;

  /* Set up the SPI mode */
  ret = ioctl(fdSPI, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_WR_MODE\n");
    close(fdSPI);
    return(1);
  }
  ret = ioctl(fdSPI, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_RD_MODE\n");
    close(fdSPI);
    return(1);
  }

  /* Set the bits per word */
  ret = ioctl(fdSPI, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_WR_BITS_PER_WORD\n");
    close(fdSPI);
    return(1);
  }
  ret = ioctl(fdSPI, SPI_IOC_RD_BITS_PER_WORD, &bitsPerWord);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_RD_BITS_PER_WORD\n");
    close(fdSPI);
    return(1);
  }

  /* Set the max speed in hz */
  ret = ioctl(fdSPI, SPI_IOC_WR_MAX_SPEED_HZ, &speedHz);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_WR_MAX_SPEED_HZ\n");
    close(fdSPI);
    return(1);
  }

  ret = ioctl(fdSPI, SPI_IOC_RD_MAX_SPEED_HZ, &speedHz);
  if (ret == -1)
  {
    fprintf(stderr, "ERROR: openSPIInterface() -> Can't set SPI_IOC_RD_MAX_SPEED_HZ\n");
    close(fdSPI);
    return(1);
  }

  /* Check the chip ID */
  id = read8(BMP183_REGISTER_CHIPID);
  if (id != 0x55)
  {
    fprintf(stderr, "BMP183 chip id does not match 0x55 (0x%02x found)\n", id);
    close(fdSPI);
    return 1;
  }

  /* Read the calibration coefficients */
  readCoefficients();

  /* Done! */
  SPIOpen = 1;
  return 0;
}

int closeSensor(void)
{
  /* Is SPI currently open? */
  if (!SPIOpen) return 1;

  /* Close SPI */
  close(fdSPI);

  /* Done! */
  SPIOpen = 0;
  return 0;
}
float getSensorTemperature(void) 
{ 
  int32_t UT, X1, X2, B5;     // following ds convention
  float t;

  UT = readRawTemperature();
  // step 1
  X1 = (UT - (int32_t)coeffs.ac6) * ((int32_t)coeffs.ac5) / pow(2,15);
  X2 = ((int32_t)coeffs.mc * pow(2,11)) / (X1+(int32_t)coeffs.md);
  B5 = X1 + X2;
  t = (B5+8)/pow(2,4);
  t /= 10;

  return t;
}

float getSensorPressure(void) 
{
  int32_t  ut = 0, up = 0, compp = 0;
  int32_t  x1, x2, b5, b6, x3, b3, p;
  uint32_t b4, b7;

  /* Get the raw pressure and temperature values */
  ut = readRawTemperature();
  up = readRawPressure();

  /* Temperature compensation */
  x1 = (ut - (int32_t)(coeffs.ac6))*((int32_t)(coeffs.ac5))/pow(2,15);
  x2 = ((int32_t)(coeffs.mc*pow(2,11)))/(x1+(int32_t)(coeffs.md));
  b5 = x1 + x2;

  /* Pressure compensation */
  b6 = b5 - 4000;
  x1 = (coeffs.b2 * ((b6 * b6) >> 12)) >> 11;
  x2 = (coeffs.ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = (((((int32_t) coeffs.ac1) * 4 + x3) << _bmp183Mode) + 2) >> 2;
  x1 = (coeffs.ac3 * b6) >> 13;
  x2 = (coeffs.b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (coeffs.ac4 * (uint32_t) (x3 + 32768)) >> 15;
  b7 = ((uint32_t) (up - b3) * (50000 >> _bmp183Mode));

  if (b7 < 0x80000000)
  {
    p = (b7 << 1) / b4;
  }
  else
  {
    p = (b7 / b4) << 1;
  }

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  compp = p + ((x1 + x2 + 3791) >> 4);

  /* Assign compensated pressure value */
  return compp;
}

