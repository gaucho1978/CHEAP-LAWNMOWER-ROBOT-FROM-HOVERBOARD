// defines the automatic navigation routines of the lawnmower


#ifndef NAVIGATOR_H
	#define NAVIGATOR_H

	#include "../Inc/config.h"

	#ifdef MASTER
		//----------------------------------------------------------------------------
		// Update INTERLOCK inputs
		//----------------------------------------------------------------------------
		void checkNavigationStatus(void);
		
		//move the lawnmower 
		void move(bool move0Straight1Turn, int16_t cmdValue );
	  void stopNavigator(void);
		bool isNavigatorRunning(void);
	#endif

#endif
