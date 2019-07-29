//configuration file
//all general settings should be defined here
//in example, you should define MASTER or SLAVE in order to compile firmware for master or slave board.
//don't touch other things if you don't know what you're doing

#ifndef CONFIG_H
	#define CONFIG_H

	#include "gd32f1x0.h"

	// ################################################################################
	#define MASTER										  	// Select if firmware is for master or slave board
	//#define SLAVE 												// Select if firmware is for master or slave board
	// ################################################################################
	#define PWM_FREQ         		16000     // PWM frequency in Hz
	#define DEAD_TIME        		60        // PWM deadtime (60 = 1µs, measured by oscilloscope)
	#define DC_CUR_LIMIT     		15        // Motor DC current limit in amps
	// ################################################################################
	#define DELAY_IN_MAIN_LOOP 	5         // Delay in ms
	#define TIMEOUT_MS          2000      // Time in milliseconds without steering commands before pwm emergency off
	// ################################################################################
	#ifdef MASTER
		#define INACTIVITY_TIMEOUT 	8        	// Minutes of not driving until poweroff (not very precise)
		// ################################################################################
		#define BAT_LOW_LVL1     35.0       // Gently beeps, show green battery symbol above this Level.
		#define BAT_LOW_LVL2     33.0       // Battery almost empty, show orange battery symbol above this Level. Charge now! 
		#define BAT_LOW_DEAD     31.0       // Undervoltage lockout, show red battery symbol above this Level.
		// ONLY DEBUG-LEVEL!!!
		//#define BAT_LOW_LVL1     29.0
		//#define BAT_LOW_LVL2     28.0
		//#define BAT_LOW_DEAD     27.0
		// ################################################################################
	#endif

	// ###### ARMCHAIR ######
	#define FILTER_SHIFT 12 						// Low-pass filter for pwm, rank k=12

	#ifdef MASTER
		#define SPEED_COEFFICIENT   -1
		#define STEER_COEFFICIENT   1
		#define REMOTE_CONTROL_PWM          //comment this line if pwm Radio CONTROL of the lawnmower is not connected
		#define DEBUG_ENABLED			
	#endif

#endif
