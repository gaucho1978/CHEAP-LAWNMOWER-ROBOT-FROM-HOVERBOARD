//defines the interface with the radio control (PWM input for speed and steer)

#include "gd32f1x0.h"
#include "../Inc/it.h"
#include "../Inc/comms.h"
#include "../Inc/commsSteeringPWM.h"
#include "../Inc/setup.h"
#include "../Inc/config.h"
#include "../Inc/defines.h"
#include "../Inc/bldc.h"
#include "stdio.h"
#include "string.h"

// Only master receives PWM remote control INPUT
#ifdef MASTER

int8_t steerPwmFiltered=-CONFIRMATIONSCOUNT;
extern int16_t steerAngle;
int32_t filter_reg3;
FlagStatus previousPWMSteerSignalLevel=RESET; //false=low signal level, true=high signal level
int16_t rc_steer_delay = 0;  //each step is 31.25usec


int8_t speedPwmFiltered=-CONFIRMATIONSCOUNT;
extern int16_t speed_mm_per_second;
int32_t filter_reg2;
FlagStatus previousPWMSpeedSignalLevel=RESET; //false=low signal level, true=high signal level
int16_t rc_speed_delay = 0;  //EACH STEP IS 31.25 usec


#define IN_RANGE(x, low, up) (((x) >= (low)) && ((x) <= (up)))

//----------------------------------------------------------------------------
// read PPM input each 31.25us
//----------------------------------------------------------------------------
void CheckPWMRemoteControlInput(void){
	
	if (gpio_input_bit_get(SPEED_PWM_PORT, SPEED_PWM_PIN)==SET){
		speedPwmFiltered++;
	}else{
		speedPwmFiltered--;
	}
	speedPwmFiltered=CLAMP(speedPwmFiltered,-CONFIRMATIONSCOUNT,CONFIRMATIONSCOUNT);

	if (gpio_input_bit_get(STEER_PWM_PORT, STEER_PWM_PIN)==SET){
		steerPwmFiltered++;
	}else{
		steerPwmFiltered--;
	}
	steerPwmFiltered=CLAMP(steerPwmFiltered,-CONFIRMATIONSCOUNT,CONFIRMATIONSCOUNT);

	
	//stop counting SPEED on faling edge if delay is between 2000 and 1000 usec
	if (speedPwmFiltered==-CONFIRMATIONSCOUNT && previousPWMSpeedSignalLevel==SET  &&  rc_speed_delay<=64 && rc_speed_delay>=32){ //IF FALLING EDGE
		previousPWMSpeedSignalLevel=RESET;
		
		filter_reg2 = filter_reg2 - (filter_reg2 >> FILTER_SHIFT2) + (int16_t)(CLAMP(rc_speed_delay-48, -16, 16)); 
		speed_mm_per_second = filter_reg2 >> FILTER_SHIFT2;
		speed_mm_per_second=MAP(speed_mm_per_second,-16,16,-250,250);  //map between -250 and 250 mm/sec // // to reach maximum speed REPLACE 250 WITH 6000 
		if(speed_mm_per_second>-40 && speed_mm_per_second<40){ //avoid unwanted movements when stick is in the middle
			speed_mm_per_second=(int16_t)0;
			gpio_bit_write(LED_GREEN_PORT, LED_GREEN, RESET); //(inverted logic since it uses npn transistor)
		}else{
				gpio_bit_write(LED_GREEN_PORT, LED_GREEN, SET); //(inverted logic since it uses npn transistor)
			// Reset the pwm timout to avoid stopping motors
			ResetTimeout();
		}
		rc_speed_delay=0;
	}

	//start counting SPEED on rising edge
	if (speedPwmFiltered==CONFIRMATIONSCOUNT && previousPWMSpeedSignalLevel==RESET){ //IF RISING EDGE
		previousPWMSpeedSignalLevel=SET;
		rc_speed_delay=0;
	}

	//stop counting STEER on falling edge
	if (steerPwmFiltered==-CONFIRMATIONSCOUNT && previousPWMSteerSignalLevel==SET && rc_steer_delay<=64 && rc_steer_delay>=32){ //IF FALLING EDGE
		previousPWMSteerSignalLevel=RESET;
		// rc_steer_delay shall be between 1000 and 2000 microseconds (32 and 64 steps).
		//make steer command smooth
		filter_reg3 = filter_reg3 - (filter_reg3 >> FILTER_SHIFT2) + (int16_t)(CLAMP(rc_steer_delay-48, -16, 16));
		steerAngle = filter_reg3 >> FILTER_SHIFT2;
		steerAngle = MAP(steerAngle,-16,16,180,0);  //map between 0 and 180 degrees
		rc_steer_delay=0;
	}

	
	//start counting STEER on rising edge
	if (steerPwmFiltered==CONFIRMATIONSCOUNT  && previousPWMSteerSignalLevel==RESET){ //IF RISING EDGE
		previousPWMSteerSignalLevel=SET;
		rc_steer_delay=0;
	}
	
	rc_speed_delay++; // each step is 31.25 usec
	rc_steer_delay++; // each step is 31.25 usec

	//after 500ms (16000 steps) declare the absence of pwm signal
	if(rc_speed_delay> 16000 || rc_steer_delay> 16000){
		speed_mm_per_second=(int16_t)0;
		steerAngle = 90;
		//shut down the blade motor
		gpio_bit_write(LED_GREEN_PORT, LED_GREEN, RESET); //(inverted logic since it uses npn transistor)
		rc_speed_delay=0;
		rc_steer_delay=0;
	}

}

#endif
