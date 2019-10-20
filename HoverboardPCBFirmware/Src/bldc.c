// defines the control of hoverboard motors

#include "gd32f1x0.h"
#include "../Inc/setup.h"
#include "../Inc/defines.h"
#include "../Inc/config.h"

	/* Choose PID parameters for slave wheel speed correction */
	// note: PID calibration according to Ziegler–Nichols method give Ku=0.2, period=3seconds(96000 samples)
	float PID_PARAM_KP        =0.3500;             /* Proporcional */
	float PID_PARAM_KI        =0.0005;        /* Integral */ 
	float PID_PARAM_KD        =0.0010;            /* Derivative */
	
#ifdef SLAVE
	#include <arm_math.h>
	extern int16_t masterRemainingSteps;
	/* PID error for slave wheel speed correction*/
	float pid_error; //int16_t
		/* Include ARM math */
	
	



	/* ARM PID Instance, float_32 format */
	//arm_pid_instance_f32 PID;
	arm_pid_instance_f32 PID;

#endif

bool overCurrent=FALSE;
bool motorIsStopping=FALSE;

uint16_t moveByStepsTimeout=0; //timeout used to maintain steady the wheel for a second, after a movementBySteps was completed
	
//const float speedConversionFactor= (((WHEEL_PERIMETER/90.0f)*6.0f)/62.5f)*3600 ; //90=steps in one turn of wheel, 6=steps in one turn of the phases, 62.5=time duration in usec for each speedcounter increment, 3600= confersion factor from mm/usec to km/h
const float speedConversionFactor= (((WHEEL_PERIMETER/90.0f))/62.5f)*3600 ; //90=steps in one turn of wheel, 62.5=time duration in usec for each speedcounter increment, 3600= confersion factor from mm/usec to km/h

// Internal constants
const int16_t pwm_res = 72000000 / 2 / PWM_FREQ; // = 2250 72M divided by 2 times the period (62,5usec)

// Global variables for voltage and current
float batteryVoltage = 40.0;
float currentDC = 0.0;
float realSpeed = 0.0;

// Timeoutvariable set by timeout timer
extern FlagStatus timedOut;

float remainingStepsFloat=0;

bool moveBySteps=FALSE;
bool moveByStepsCompleted=FALSE;
bool printMovementCompleted=FALSE;
int16_t remainingSteps=0;

//int16_t 
float tmpPwmCorrectionFactor=0; //used only by slave wheel, to adjust its speed
int16_t pwmCorrectionFactor=0; //used only by slave wheel, to adjust its speed

int8_t approachingCorrectionFactor=0; //used to reduce the speed when arriving to final destination
// Variables to be set from the main routine
int16_t bldc_inputFilterPwm = 0;

FlagStatus bldc_enable = RESET;

// ADC buffer to be filled by DMA
adc_buf_t adc_buffer;

// Internal calculation variables
uint8_t hall_a;
uint8_t hall_b;
uint8_t hall_c;
uint8_t hall;

int y = 0;     // yellow = phase A
int b = 0;     // blue   = phase B
int g = 0;     // green  = phase C


uint8_t pos;
uint8_t lastPos=0;
int16_t bldc_outputFilterPwm = 0;
int32_t filter_reg;
FlagStatus buzzerToggle = RESET;
uint8_t buzzerFreq = 0;
uint8_t buzzerPattern = 0;
uint16_t buzzerTimer = 0;
int16_t offsetcount = 0;
int16_t offsetdc = 2000;
uint32_t speedCounter = 0;

//----------------------------------------------------------------------------
// Commutation table
//----------------------------------------------------------------------------
const uint8_t hall_to_pos[8] =
{
	// annotation: for example SA=0 means hall sensor pulls SA down to Ground
  0, // hall position [-] - No function (access from 1-6) 
  3, // hall position [1] (SA=1, SB=0, SC=0) -> PWM-position 3
  5, // hall position [2] (SA=0, SB=1, SC=0) -> PWM-position 5
  4, // hall position [3] (SA=1, SB=1, SC=0) -> PWM-position 4
  1, // hall position [4] (SA=0, SB=0, SC=1) -> PWM-position 1
  2, // hall position [5] (SA=1, SB=0, SC=1) -> PWM-position 2
  6, // hall position [6] (SA=0, SB=1, SC=1) -> PWM-position 6
  0, // hall position [-] - No function (access from 1-6) 
};



//----------------------------------------------------------------------------
// Block PWM calculation based on position
// if stopPosition is true, the motor is stopped exactly in the same actual position (HALL position)
//----------------------------------------------------------------------------
__INLINE void blockPWM(int pwm, int pwmPos, int *y, int *b, int *g, bool stopPosition){
  switch(pwmPos){
    case 1:
			*y = 0;
			*b = pwm;
			*g = -pwm;
			if(stopPosition){ //pwm positive value means that phase is rotating clockwise
					*y=0;
					*b=ABS(pwm)*2; 
					*g=ABS(pwm)*2;					
				
			}
			break;
    case 2:
			*y = -pwm;
			*b = pwm;
			*g = 0;
			if(stopPosition){ //pwm positive value means that phase is rotating clockwise
				*y=-ABS(pwm)*2;
				*b = -ABS(pwm)*2;
				*g =0;
			}
      break;
    case 3:
      *y = -pwm;
			*b = 0;
			*g = pwm;
      if(stopPosition){ //pwm positive value means that phase is rotating clockwise
				*y =ABS(pwm)*2;
				*b =0;
				*g =ABS(pwm)*2;
			}
			break;
    case 4:
      *y = 0;
			*b = -pwm;
			*g = pwm;
      if(stopPosition){ //pwm positive value means that phase is rotating clockwise
				*y = 0;
				*b = -ABS(pwm)*2;
				*g = -ABS(pwm)*2;			
			}
			break;
    case 5:
     	*y = pwm;
			*b = -pwm;
			*g = 0;
      if(stopPosition){ //pwm positive value means that phase is rotating clockwise
				*y = ABS(pwm)*2;
				*b = ABS(pwm)*2;
				*g = 0;
			}
			break;
    case 6:
     	*y = pwm;
			*b = 0;
			*g = -pwm;
      if(stopPosition){ //pwm positive value means that phase is rotating clockwise
				*y = -ABS(pwm)*2;
				*b = 0;
				*g = -ABS(pwm)*2;
 			}
			break;
    default:
      *y = 0;
      *b = 0;
      *g = 0;
  }
}

#ifdef SLAVE
void PID_init(void){
			/* Set PID parameters for slave wheel speed correction*/
			/* Set this for your needs */
			PID.Kp = PID_PARAM_KP;		/* Proporcional */
			PID.Ki = PID_PARAM_KI;		/* Integral */
			PID.Kd = PID_PARAM_KD;		/* Derivative */
			/* Initialize PID system */
			arm_pid_init_f32(&PID, 1);
}

void PID_setKp(float myK){
			PID_PARAM_KP = myK;		/* Proporcional */
			PID_init();
}
void PID_setKi(float myK){
			PID_PARAM_KI = myK;		
			PID_init();
}
void PID_setKd(float myK){
			PID_PARAM_KD = myK;		
			PID_init();
}


#endif
//----------------------------------------------------------------------------
// Set motor enable
//----------------------------------------------------------------------------
void SetEnable(FlagStatus setEnable){
	bldc_enable = setEnable;
}

//----------------------------------------------------------------------------
// Set pwm -1000 to 1000
//----------------------------------------------------------------------------
void SetPWM(int16_t setPwm){
	bldc_inputFilterPwm = CLAMP(setPwm, -1000, 1000);
}

//----------------------------------------------------------------------------
// Calculation-Routine for BLDC => calculates with 16kHz
//----------------------------------------------------------------------------
void CalculateBLDC(void){

	// Calibrate ADC offsets for the first 1000 cycles
  if (offsetcount < 1000){  
    offsetcount++;
    offsetdc = (adc_buffer.current_dc + offsetdc) / 2;
    return;
  }
	
	// Calculate battery voltage every 100 cycles
  if (buzzerTimer % 100 == 0){
    batteryVoltage = batteryVoltage * 0.999 + ((float)adc_buffer.v_batt * ADC_BATTERY_VOLT) * 0.001;
  }
	
	#ifdef MASTER
		// Create square wave for buzzer
		buzzerTimer++;
		if (buzzerFreq != 0 && (buzzerTimer / 5000) % (buzzerPattern + 1) == 0){
			if (buzzerTimer % buzzerFreq == 0){
				buzzerToggle = buzzerToggle == RESET ? SET : RESET; // toggle variable
				gpio_bit_write(BUZZER_PORT, BUZZER_PIN, buzzerToggle);
			}
		}else{
			gpio_bit_write(BUZZER_PORT, BUZZER_PIN, RESET);
		}
	#endif
	
	// Calculate current DC
	currentDC = ABS((adc_buffer.current_dc - offsetdc) * MOTOR_AMP_CONV_DC_AMP);

  // Disable PWM when current limit is reached (current chopping), enable is not set or timeout is reached
	if (currentDC > DC_CUR_LIMIT || bldc_enable == RESET || timedOut == SET){
		timer_automatic_output_disable(TIMER_BLDC);		
		if(currentDC > DC_CUR_LIMIT){
			overCurrent=TRUE;
		}
  }else{
		timer_automatic_output_enable(TIMER_BLDC);
  }
	
	
  // Read hall sensors
	hall_a = gpio_input_bit_get(HALL_A_PORT, HALL_A_PIN);
  hall_b = gpio_input_bit_get(HALL_B_PORT, HALL_B_PIN);
	hall_c = gpio_input_bit_get(HALL_C_PORT, HALL_C_PIN);
  
	// Determine current position based on hall sensors
  hall = hall_a * 1 + hall_b * 2 + hall_c * 4;
	pos = hall_to_pos[hall]; //pos will be at 90 degrees counterclockwise from hall position
	
			
	
	if(moveBySteps){
		if(moveByStepsCompleted){
			// if movement was completed, remain with the wheel steady for a while
			//wait at last 1 second, then return to standard commands speed and steer
			//if timeout was not started, start it
			moveByStepsTimeout+=1; //one cycle duration is around 62,5usec
			//if timeout is reached return to standard commands input
			if (moveByStepsTimeout>24000){ //1,5 seconds
				SetPWM(0);
			}
			if (moveByStepsTimeout>32000){ //2 seconds
				moveBySteps=FALSE;
				moveByStepsTimeout=0;
			}	
		}else{//movement controlled by steps not completed
			#ifdef SLAVE
				//check if the remaining steps of master wheel is the same of slave wheel
				pid_error=remainingSteps-masterRemainingSteps; //error positive means that slave wheel must go faster
				/* Calculate PID here, argument is error */
				/* Output data will be returned, we will use it as correction parameter */
				
				tmpPwmCorrectionFactor = arm_pid_f32(&PID, pid_error); //correction positive increase speed of slave wheel
				pwmCorrectionFactor=tmpPwmCorrectionFactor+(tmpPwmCorrectionFactor/ABS(tmpPwmCorrectionFactor))*0.5;
				if (bldc_inputFilterPwm<0) pwmCorrectionFactor=-pwmCorrectionFactor; //set same sense of rotation of wheel and correction
				if (bldc_inputFilterPwm==0 ) pwmCorrectionFactor=0;			
				if(pwmCorrectionFactor>900) pwmCorrectionFactor=900;
				if(pwmCorrectionFactor<-900) pwmCorrectionFactor=-900;
			
				// Note: To transmit 9 bytes at 115200bps the boards requires 6,9msec
				// in 6,9msec the wheel, at around 0,6km/h crosses less than one step of the wheel.
				// This means that the serial line transmission introduces a negligible delay.
			#endif
			
			#ifdef MASTER //slave wheel don't need this, cause there is the pwmCorrectionFactor and if used together generates oscillations
				if(remainingSteps<40 ){ //if we are arriving to final position, go slow
					approachingCorrectionFactor=(-realSpeed+0.1)*100;
					if(bldc_inputFilterPwm<0) approachingCorrectionFactor=-approachingCorrectionFactor;
				}
			#endif
			#ifdef SLAVE //slave wheel just go slower
				if(remainingSteps==40 ){ //if we are arriving to final position, go slow
					approachingCorrectionFactor=-bldc_inputFilterPwm/4;
				}
			#endif
			
		}
	}

	
	// Calculate low-pass filter for pwm value
	filter_reg = filter_reg - (filter_reg >> FILTER_SHIFT) + bldc_inputFilterPwm+approachingCorrectionFactor+pwmCorrectionFactor;
	bldc_outputFilterPwm = filter_reg >> FILTER_SHIFT;
	 
	
	if(!(moveBySteps && moveByStepsCompleted )){ //if we completed moveBySteps (or if we are completing it) we don't have to move the wheel, so we must maintain the wheel steady, so we don't have to change y,b,g. otherwise, update the phase of the motor
		// Update PWM channels based on position y(ellow), b(lue), g(reen)
		blockPWM(bldc_outputFilterPwm, pos, &y, &b, &g,FALSE); //set phases 90 degrees forward
	} 
	
	
	
	
	if(moveBySteps && (ABS(pos-lastPos)==1 || ABS(pos-lastPos)==5  )){ //if the wheel rotated by a position
		if(bldc_outputFilterPwm<0){ //if phase is rotating counterclockwise 
			if(((pos-lastPos)==1) || ((pos-lastPos)==-5)){ // and if we rotated one step counterclockwise
				remainingSteps-=1;
			}else{
				remainingSteps+=1;
			}
		}else{ //if phase is rotating clockwise
			if(((pos-lastPos)==1) || ((pos-lastPos)==-5)){ // and if we rotated one step counterclockwise
				remainingSteps+=1;
			}else{
				remainingSteps-=1;
			}
		}
		if(remainingSteps==0){
			moveByStepsCompleted=TRUE;
			printMovementCompleted=TRUE;
			blockPWM((ABS(bldc_outputFilterPwm)/bldc_outputFilterPwm)*100, pos, &y, &b, &g,TRUE); //stop to actual position(90 degrees from pos), with double force
			approachingCorrectionFactor=0;
		}
	}

	// Increments with 62.5us
	if(speedCounter < 1555) speedCounter++;// No speed after 250ms
	
	
	//calculate real speed each movement of the wheel
	if(ABS(pos-lastPos)==1 || ABS(pos-lastPos)==5  ){ //if the wheel rotated by a position
		realSpeed = speedConversionFactor / (float)speedCounter; //[km/h]
		speedCounter = 0;
	}else{
		if(realSpeed > (speedConversionFactor / (float)speedCounter)){ //[km/h]
			realSpeed = speedConversionFactor / (float)speedCounter; //[km/h]
		}
		if (speedCounter >= 1555) realSpeed = 0;
	}
	
	// Save last position
	lastPos = pos;
	
	// Set PWM output (pwm_res/2 is the mean value, setvalue has to be between 10 and pwm_res-10)
	timer_channel_output_pulse_value_config(TIMER_BLDC, TIMER_BLDC_CHANNEL_G, CLAMP(g + pwm_res / 2, 10, pwm_res-10));
	timer_channel_output_pulse_value_config(TIMER_BLDC, TIMER_BLDC_CHANNEL_B, CLAMP(b + pwm_res / 2, 10, pwm_res-10));
	timer_channel_output_pulse_value_config(TIMER_BLDC, TIMER_BLDC_CHANNEL_Y, CLAMP(y + pwm_res / 2, 10, pwm_res-10));

}

void go_to(int16_t distanceInMillimeters){
	// perimeter of the wheel:54,5cm
	remainingStepsFloat=(float)ABS(distanceInMillimeters)/WHEEL_PERIMETER;
	remainingStepsFloat=remainingStepsFloat * 90.0f; //90 steps for each rotation of the wheel
	remainingStepsFloat=remainingStepsFloat + 0.5f;
	remainingSteps=(int16_t)remainingStepsFloat;
	
	if(remainingSteps==0) return; //abort
	//start
	lastPos=pos;
	bldc_enable = SET;
	moveByStepsCompleted=FALSE;
	moveByStepsTimeout=0;
	//arm_pid_reset_q15(&PID);
	#ifdef SLAVE
		PID_init();
		masterRemainingSteps=remainingSteps; //avoid the slave wheel to rotate fast at the beginning, before to receive message from master board
	#endif
	motorIsStopping=FALSE;
	moveBySteps=TRUE;
	approachingCorrectionFactor=0;
	if(distanceInMillimeters>=0){//1=forward
		#ifdef MASTER
			SetPWM(-100);
		#endif
		#ifdef SLAVE
			SetPWM(100);
		#endif
	}else{ //0=backward
		#ifdef MASTER
			SetPWM(100);
		#endif
		#ifdef SLAVE
			SetPWM(-100);
		#endif
	}
}
