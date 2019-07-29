/*
  Defines the serial 232 interface of the SLAVE BOARD named REMOTE on the PCB wiring diagram.
  the protocol was defined by Florian to connect an external steering device (bluetooth interface). 
	The protocol is defined on the excel sheet included in the project folder
*/

#ifndef COMMSBLUETOOTH_H
#define COMMSBLUETOOTH_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

// Only slave communicates over bluetooth
#ifdef SLAVE

//----------------------------------------------------------------------------
// Update USART bluetooth input
//----------------------------------------------------------------------------
void UpdateUSARTBluetoothInput(void);

#endif

#endif
