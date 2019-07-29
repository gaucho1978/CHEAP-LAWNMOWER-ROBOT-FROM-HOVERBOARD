//initializes all interrupts, timers, digital and analog pins of the board


#ifndef SETUP_H
#define SETUP_H

#include "gd32f1x0.h"
#include "../Inc/config.h"


#define USART_MASTERSLAVE_RX_BUFFERSIZE 1
#define USART_MASTERSLAVE_DATA_RX_ADDRESS ((uint32_t)0x40004424)

#define USART_REMOTE_COM_RX_BUFFERSIZE 1
#define USART_REMOTE_COM_DATA_RX_ADDRESS ((uint32_t)0x40013824)

#define M_PI 3.14159265358979323846
//----------------------------------------------------------------------------
// Initializes the interrupts
//----------------------------------------------------------------------------
void Interrupt_init(void);

//----------------------------------------------------------------------------
// Initializes the watchdog
//----------------------------------------------------------------------------
ErrStatus Watchdog_init(void);

//----------------------------------------------------------------------------
// Initializes the timeout timer
//----------------------------------------------------------------------------
void TimeoutTimer_init(void);

//----------------------------------------------------------------------------
// Initializes the GPIOs
//----------------------------------------------------------------------------
void GPIO_init(void);

//----------------------------------------------------------------------------
// Initializes the PWM
//----------------------------------------------------------------------------
void PWM_init(void);

//----------------------------------------------------------------------------
// Initializes the ADC
//----------------------------------------------------------------------------
void ADC_init(void);

//int i2c to communicate with accelerometer
void I2C_IMU_init(void);
//----------------------------------------------------------------------------
// Initializes the usart master slave
//----------------------------------------------------------------------------
void USART_MasterSlave_init(void);

//----------------------------------------------------------------------------
// Initializes the steer/bluetooth usart
//----------------------------------------------------------------------------
void USART_REMOTE_COM_init(void);

#endif
