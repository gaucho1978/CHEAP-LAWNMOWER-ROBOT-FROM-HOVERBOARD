// defines the control of hoverboard motors

#ifndef BLDC_H
#define BLDC_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

#ifdef SLAVE
	//initialize PID to control speed of the slave wheel
	void PID_init(void);
	void PID_setKp(float myK);
	void PID_setKi(float myK);
	void PID_setKd(float myK);
#endif
//----------------------------------------------------------------------------
// Set motor enable
//----------------------------------------------------------------------------
void SetEnable(FlagStatus setEnable);

//----------------------------------------------------------------------------
// Set pwm -1000 to 1000
//----------------------------------------------------------------------------
void SetSpeed(int16_t setspeed);

//----------------------------------------------------------------------------
// Calculation-Routine for BLDC => calculates with 16kHz
//----------------------------------------------------------------------------
void CalculateBLDC(void);

//move of distanceInCentimeters in a direction (distance > or equal to 0 means forward, otherwise backward)
void go_to(int16_t distanceInCentimeters);
#endif
