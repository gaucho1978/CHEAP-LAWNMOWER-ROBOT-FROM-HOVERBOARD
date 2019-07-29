// defines the control of hoverboard motors

#ifndef BLDC_H
#define BLDC_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

//----------------------------------------------------------------------------
// Set motor enable
//----------------------------------------------------------------------------
void SetEnable(FlagStatus setEnable);

//----------------------------------------------------------------------------
// Set pwm -1000 to 1000
//----------------------------------------------------------------------------
void SetPWM(int16_t setPwm);

//----------------------------------------------------------------------------
// Calculation-Routine for BLDC => calculates with 16kHz
//----------------------------------------------------------------------------
void CalculateBLDC(void);

#endif
