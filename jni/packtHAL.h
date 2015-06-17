/**********************************************************
  Main PacktHAL header file

  Written by Andrew Henderson (hendersa@icculus.org)

  This code is made available under the BSD license.
**********************************************************/

#ifndef __PACKTHAL_H__
#define __PACKTHAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Library init/shutdown */
extern int openPacktHAL(void);
extern int closePacktHAL(void);

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

/* I2C FRAM interfacing functions */
extern int openFRAM(const unsigned int bus, const unsigned int address);
extern int writeFRAM(const unsigned int offset, const unsigned int bufferSize,
  const char *buffer); 
extern int readFRAM(const unsigned int offset, const unsigned int bufferSize,
  const char *buffer);
extern int closeFRAM(void);

/* SPI BMP183 interfacing functions */
extern int openSensor(void);
extern float getSensorTemperature(void);
extern float getSensorPressure(void);
extern int closeSensor(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PACKTHAL_H__ */

