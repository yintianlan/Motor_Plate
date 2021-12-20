/*************  功能说明    **************

******************************************/
#define	_TRAVEL_SWITCH_GLOBAL_

#include "TravelSwitch.H"
#include <stdio.h>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "Public.h"
#include "MotorIO.H"
#include "Led.H"



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
**Name:		 	TravelJudge
**Function:	 	判断运行行程
**Args:		None
**Return:	None
******************************************************************************/
void TravelJudge(void)
{
	upTravelFlag = (IO_SWITCH1 == SWITCH_ON) ? TRUE : FALSE;
	downTravelFlag = (IO_SWITCH2 == SWITCH_ON) ? TRUE : FALSE;
	
	if( (upTravelFlag == TRUE) && (downTravelFlag != TRUE) )
	{
		currentTravel = T_UP;			// 已经到顶部
	}
	else if( (upTravelFlag == TRUE) && (downTravelFlag != TRUE) )
	{
		currentTravel = T_DOWN;			// 已经到底部
	}
	else if( (upTravelFlag != TRUE) && (downTravelFlag != TRUE) )
	{
		if(previousTravel == T_DOWN) {
			currentTravel = T_RISING;	// 上升中
		} 
		else if(previousTravel == T_UP) {
			currentTravel = T_DECLINING;// 下降中
		}
	}
	
	if( previousTravel != currentTravel) 
	{
		previousTravel = currentTravel;//记录前一个状态
		printf("previousTravel: %02bX\r\n", previousTravel);
	}
}

/*****************************************************************************
**Name:		 	TravelDealWith
**Function:	 	处理行程操作
**Args:		None
**Return:	None
******************************************************************************/
void TravelDealWith(uchar state)
{
	switch(state) {
		case T_ORIGINAL:
				LedDisplayPrg(SYSTEM_LED, LED_W);
			break;

		case T_DOWN:
				LedDisplayPrg(SYSTEM_LED, LED_W);
			break;
		
		case T_RISING:
				LedDisplayPrg(SYSTEM_LED, LED_R);
			break;
		
		case T_UP:
				LedDisplayPrg(SYSTEM_LED, LED_B);
			break;
		
		case T_DECLINING:
				LedDisplayPrg(SYSTEM_LED, LED_Y);
			break;
				
		default :
			break;
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
	TravelJudge();
	TravelDealWith(currentTravel);
}

#endif
