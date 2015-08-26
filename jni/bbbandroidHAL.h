/**********************************************************
  Main bbbandroidHAL header file

  Written by Andrew Henderson (hendersa@icculus.org)
  Modified by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file bbbandroidHAL.h
 * @author Ankur Yadav (ankurayadav@gmail.com), Andrew Henderson (hendersa@icculus.org)
 * @brief Main bbbandroidHAL header file
 */

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
extern int pwmSetPeriod(const uint8_t channel, const uint32_t period_ns);
extern int pwmGetPeriod(const uint8_t channel);
extern int pwmSetDutyCycle(const uint8_t channel, const uint32_t duration_ns);
extern int pwmGetDutyCycle(const uint8_t channel);
extern int pwmSetPolarity(const uint8_t channel, const uint8_t polarity);
extern int pwmGetPolarity(const uint8_t channel);
extern int pwmRun(const uint8_t channel);
extern int pwmStop(const uint8_t channel);
extern int pwmRunCheck(const uint8_t channel);

/* ADC interfacing functions */
extern int readADC(const uint8_t channel);

/* I2C interfacing functions */
extern int i2cOpenAdaptor(const uint8_t adaptorNumber);
extern int i2cSetSlave(const int i2cFD, const uint8_t address);
extern void i2cClose(const int i2cFD);
extern int i2cSetAddress(const int i2cFD, const uint8_t add);
extern int i2cWriteByte(const int i2cFD, const uint8_t add, const uint8_t byte);
extern int i2cWriteBytes(const int i2cFD, const uint8_t add, const int length, const uint8_t *bytes);
extern int i2cReadByte(const int i2cFD, const uint8_t add);
extern int i2cReadBytes(const int i2cFD, const uint8_t add, const int length, uint8_t *buff);

/* SPI interfacing functions */
extern int spiTransfer(const int spiFD, const uint8_t tx[], const uint8_t rx[], const int len);
extern int spiOpen(const uint8_t bus, const uint8_t device, const uint32_t speed, const uint8_t mode, const uint8_t bpw);
extern int spiReadByte(const int spiFD, const uint8_t regAdd);
extern unsigned char* spiReadBytes(const int spiFD, const int len, const uint8_t startAdd);
extern int spiWriteRegByte(const int spiFD, const uint8_t regAdd, const uint8_t data);
extern int spiWriteBytes(const int spiFD, const uint8_t data[], const int len);
extern int spiSetMode(const int spiFD, const uint8_t mode);
extern int spiSetSpeed(const int spiFD, const uint32_t speed);
extern int spiSetBitsPerWord(const int spiFD, const uint8_t bpw);
extern void spiClose(const int spiFD);

/* CAN interfacing functions */
extern int canOpenSocket(const int socket_type, const int protocol);
extern int canOpenRaw(const char *port);
extern unsigned char* canReadBytes(const int fd, int *length);
extern int canSendBytes(const int canFD, const int no_bytes, const uint8_t data[]);
extern void canClose(const int canFD);

/* UART interfacing functions */
extern int uartOpen(const uint8_t device, const uint32_t bdrate);
extern int uartWrite(const int uartFD, const int length, const uint8_t *bytes);
extern int uartRead(const int uartFD, const int length, uint8_t *bytes);
extern void uartClose(const int uartFD);

/* USB interfacing functions */
extern ssize_t usbInit();
extern void usbClose();
extern int usbGetDevices(int ids[][8], unsigned char strings[][3][256]);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __BBBANDROIDHAL_H__ */