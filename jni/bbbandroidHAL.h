/**********************************************************
  Main bbbandroidHAL header file

  Written by Andrew Henderson (hendersa@icculus.org)
  Modified by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

#include <stdio.h>

#ifndef __BBBANDROIDHAL_H__
#define __BBBANDROIDHAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Library init/shutdown */
extern int openBBBAndroidHAL(void);
extern int closeBBBAndroidHAL(void);

/* GPIO interfacing functions */
extern int openGPIO(const int useMmap);
extern int readGPIO(const unsigned int header, const unsigned int pin);
extern int writeGPIO(const unsigned int header, const unsigned int pin,
const unsigned int value);
extern void closeGPIO(void);

/* PWM interfacing functions */
extern int pwmSetPeriod(unsigned int channel, unsigned int period_ns);
extern unsigned int pwmGetPeriod(unsigned int channel);
extern int pwmSetDutyCycle(unsigned int channel, unsigned int duration_ns);
extern int pwmGetDutyCycle(unsigned int channel);
extern int pwmSetPolarity(unsigned int channel, unsigned int polarity);
extern int pwmGetPolarity(unsigned int channel);
extern int pwmRun(unsigned int channel);
extern int pwmStop(unsigned int channel);
extern int pwmRunCheck(unsigned int channel);

/* ADC interfacing functions */
extern uint32_t readADC(unsigned int channel);

/* I2C interfacing functions */
extern int i2cOpenAdaptor(uint8_t adaptorNumber);
extern int i2cSetSlave(int i2cFD, uint8_t address);
extern void i2cClose(int i2cFD);
extern int i2cSetAddress(int i2cFD, unsigned char add);
extern int i2cWriteByte(int i2cFD, unsigned char add, unsigned char byte);
extern int i2cWriteBytes(int i2cFD, unsigned char add, int length, uint8_t *bytes);
extern int i2cReadByte(int i2cFD, unsigned char add);
extern int i2cReadBytes(int i2cFD, unsigned char add, int length, uint8_t *buff);

/* SPI interfacing functions */
extern int spiOpen(unsigned int bus, unsigned int device, uint32_t speed, uint8_t mode, uint8_t bpw);
extern unsigned char spiReadByte(int spiFD, unsigned int regAdd);
extern unsigned char* spiReadBytes(int spiFD, unsigned int len, unsigned int startAdd);
extern int spiWriteRegByte(int spiFD, unsigned int regAdd, unsigned char data);
extern int spiWriteBytes(int spiFD, unsigned char data[], int len);
extern int spiSetMode(int spiFD, uint8_t mode);
extern int spiSetSpeed(int spiFD, uint32_t speed);
extern int spiSetBitsPerWord(int spiFD, uint8_t bpw);
extern void spiClose(int spiFD);

/* CAN interfacing functions */
extern int canOpenSocket(int socket_type, int protocol);
extern int canOpenRaw(const char *port);
extern unsigned char* canReadBytes(int fd, int *length);
extern int canSendBytes(int fd, unsigned int no_bytes, unsigned char data[]);
extern void canClose(int fd);

/* UART interfacing functions */
extern int uartOpen(unsigned int device, unsigned int bdrate);
extern int uartWrite(int uartFD, int length, unsigned char *bytes);
extern int uartRead(int uartFD, int length, unsigned char *bytes);
extern void uartClose(int uartFD);

/* USB interfacing functions */
extern ssize_t usbInit();
extern void usbClose();
extern int usbGetDevices(int ids[][8], unsigned char strings[][3][256]);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __BBBANDROIDHAL_H__ */