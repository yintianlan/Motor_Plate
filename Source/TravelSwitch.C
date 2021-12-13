/*************  功能说明    **************

******************************************/
#define	_TRAVEL_SWITCH_GLOBAL_

#include "TravelSwitch.H"
#include <stdio.h>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "Public.h"
#include "MotorDirver.H"


/*****************************************************************************
**Name:		 	UpTravelSwitch
**Function:	 	上行程开关处理
**Args:		None
**Return:	None
******************************************************************************/
void UpTravelSwitch(void)	
{
	//电机已到顶部
	if(IO_SWITCH1 == SWITCH_ON) {
	
	} else {
	
	}
}

/*****************************************************************************
**Name:		 	DownTravelSwitch
**Function:	 	下行程开关处理
**Args:		None
**Return:	None
******************************************************************************/
void DownTravelSwitch(void)	
{
	//电机已到底部
	if(IO_SWITCH2 == SWITCH_ON) {
	
	} else {
	
	}
}

/*****************************************************************************
**Name:		 	TravelSwitchPrg
**Function:	 	限位开关处理
**Args:		None
**Return:	None
******************************************************************************/
void TravelSwitchPrg(void)
{
	UpTravelSwitch();
	DownTravelSwitch();
}

#endif
