//defines the intrface with the accelerometer chip (i2c serial line)


#ifndef COMMSACCELEROMETER_H
	#define COMMSACCELEROMETER_H
	#include "gd32f1x0.h"
	#include "../Inc/config.h"
	#include <math.h> 
	// Only master NEEDS TO COMMUNICATE WITH ACCELEROMETER
#ifdef MASTER
	//----------------------------------------------------------------------------
	// GET ACCELEROMETER DATA
	//----------------------------------------------------------------------------
	void GetAccelerometerData(void);
#endif

#endif
