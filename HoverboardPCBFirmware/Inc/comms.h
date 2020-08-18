// Defines general function to send data via 232 serial port, and to check crc of received/transmitted data.
// this was defined by Florian on its firmware. no change on this.


#ifndef COMMS_H
#define COMMS_H

#include "gd32f1x0.h"
#include "../Inc/config.h"

//----------------------------------------------------------------------------
// Send buffer via USART
//----------------------------------------------------------------------------
void SendBuffer(uint32_t usart_periph, uint8_t buffer[], uint8_t length);

//----------------------------------------------------------------------------
// Calculate CRC
//----------------------------------------------------------------------------
uint16_t CalcCRC(uint16_t  crc, uint8_t *ptr, int count);

#endif
