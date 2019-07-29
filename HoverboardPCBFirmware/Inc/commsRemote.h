/*
  defines the serial 232 interface of the MASTER BOARD named REMOTE on the PCB wiring diagram.
	this serial line is used for debug purposes, if connected to a terminal pc, or it is commonly used to communicate with sonar sensors via arduino board.
*/

#ifndef COMMSREMOTE_H
#define COMMSREMOTE_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

// Only master communicates over REMOTE USART for debug purposes (terminal) or sonar sensors (arduino board)
#ifdef MASTER
	//----------------------------------------------------------------------------
	// Analyze received USART input
	//----------------------------------------------------------------------------
	void UpdateUSART_REMOTE_MASTER_BOARD_Input(void);

	//----------------------------------------------------------------------------
	// Send message via REMOTE USART device
	//----------------------------------------------------------------------------
	void Send_Data_over_REMOTE_serialPort_of_MasterBoard(void);
#endif

void myFtoa(float n, char *res, int afterpoint) ;

#endif
