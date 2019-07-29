// routine for automatic navigation of the lawnmower on the garden

#include "../Inc/config.h"
#include "../Inc/defines.h"


// Only master has navigator
#ifdef MASTER

extern FlagStatus it_is_Raining;
extern float pitchAngle;
extern float rollAngle;
extern float yawAngle;

	void checkNavigationStatus(void){
	}

#endif
