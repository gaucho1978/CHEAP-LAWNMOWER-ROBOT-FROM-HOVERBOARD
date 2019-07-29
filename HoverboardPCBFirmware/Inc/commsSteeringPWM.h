//defines the interface with the radio control (PWM input for speed and steer)


#ifndef COMMSSTEERINGPWM_H
#define COMMSSTEERINGPWM_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

// Only master receives remote control PWM INPUT
#ifdef MASTER
//----------------------------------------------------------------------------
// Update PWM SPEED steer input
//----------------------------------------------------------------------------
void CheckPWMRemoteControlInput(void);


#endif

#endif
