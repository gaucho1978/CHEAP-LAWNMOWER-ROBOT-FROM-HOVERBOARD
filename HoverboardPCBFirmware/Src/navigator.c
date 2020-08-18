// routine for automatic navigation of the lawnmower on the garden

#include "../Inc/config.h"
#include "../Inc/defines.h"
#include "../Inc/setup.h"
#include "../Inc/commsMasterSlave.h"
#include "../Inc/bldc.h"

// Only master has navigator
#ifdef MASTER

extern FlagStatus it_is_Raining;
extern bool slaveBoardMovementByStepCompleted;
extern bool moveBySteps;
extern bool moveByStepsCompleted;
extern float pitchAngle;
extern float rollAngle;
extern float yawAngle;
extern int16_t speed_mm_per_second;
uint8_t jumpsCounter=0;
bool navigatorStopped=TRUE;

typedef struct
{ 
    bool move0Straight1Turn;     // defines if we shall go straight or turn
    int16_t cmdValue;            // if we are going straight it indicates direction (forward positive values) in millimeters
																 //if we are turning it indicates angle in degrees (turns right por positive values)
}move_struct;

int16_t programPointer=0;

move_struct pathArray[8]={{FALSE,500},{TRUE,-90},{FALSE,500},{TRUE,-90},{FALSE,500},{TRUE,-90},{FALSE,500},{TRUE,-90}};
move_struct pathArray2[9]={{TRUE,360},{TRUE,360},{TRUE,-360},{TRUE,-360},{FALSE,1},{FALSE,10000},{FALSE,-10000},{FALSE,10000},{FALSE,-10000}};
move_struct pathArray3[9]={{FALSE,-10000},{FALSE,10000},{FALSE,-10000},{FALSE,10000},{FALSE,-10000},{FALSE,10000},{FALSE,-10000},{FALSE,10000},{FALSE,-10000}};

//move_struct pathArray2[9]={{FALSE,WHEEL_PERIMETER},{FALSE,WHEEL_PERIMETER},{FALSE,-WHEEL_PERIMETER},{FALSE,-WHEEL_PERIMETER},{FALSE,1},{TRUE,360},{TRUE,360},{TRUE,-360},{TRUE,-360}};

	void move(bool move0Straight1Turn, int16_t cmdValue ){
		float tmpDistance;
		//if we go straight, cmdValue is distance in mm (positive sign moves forward, backward otherwise.)
		//if we turn, cmdValue is the number of degrees of rotation (positive sign turns right, left otherwise)
		
		if(move0Straight1Turn){ //we must turn
			if (ABS(cmdValue)>360) cmdValue=0; //possible error in message, set 0 to avoid wrong movement
			//half distance for each wheel
			// DISTANCE_BETWEEN_WHEELS is the diameter
			// during 360 degrees each wheel shall run on a circle with perimeter length PI * DISTANCE_BETWEEN_WHEELS
			tmpDistance=DISTANCE_BETWEEN_WHEELS/360.0f;
			tmpDistance=tmpDistance* M_PI;
			tmpDistance= tmpDistance * (float)cmdValue;
			SendSlave(0, RESET, RESET, SET, 3, (int16_t)tmpDistance); //go forward to turn right
			go_to((int16_t)-tmpDistance); //go backward to turn right
		}else{ //go straight
			//sendSlave parameters: pwm value, enable motors cmd, shutoff cmd, charging status,id(3=distance), value in mm
			SendSlave(0, RESET, RESET, SET, 3, cmdValue); //go straight with slave wheel
			go_to(cmdValue); //go straight with master wheel
		}
	}

	
	void checkNavigationStatus(void){
		
		if (navigatorStopped) return;
		//execute program as soon as we can
		//if we are not moving by steps
		// or if we are movingbysteps and we completed now a movement with both wheels
		
		if((moveBySteps==FALSE) || (moveBySteps && moveByStepsCompleted==TRUE && slaveBoardMovementByStepCompleted==TRUE)){ 
			//if we completed the program, just return
			if(programPointer>8) return;
			
			jumpsCounter+=1;
			if(jumpsCounter<5){ //execution each 5 seconds
				return;
			}
			jumpsCounter=0;
		
			//move it!
			move(pathArray[programPointer].move0Straight1Turn, pathArray[programPointer].cmdValue);
			programPointer++;
			//move(FALSE,(int16_t)1000);
		}
	}
	
	void stopNavigator(void){
		navigatorStopped=TRUE;
		SendSlave(0, RESET, RESET, SET, 3, (int16_t)10);
		go_to(10); 
		
	}
	
	bool isNavigatorRunning(void){
		if (navigatorStopped) return FALSE;
		if (programPointer>= sizeof(pathArray)/3 ) return FALSE;
		return TRUE;
	}

#endif
