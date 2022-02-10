/*************  功能说明    **************
板上需要控制的led有，如下：
*面板上的LED —— 按键指示灯
	不亮：
	常亮：
	闪烁：

*双色灯  —— 系统状态指示
	不亮：
	红色：
	蓝色：
	黄色：

*网口灯 —— 串口通信指示
1、网口一般有两个信号灯，一个代表连接状态指示灯，另一个代表信号传输指示灯
2、网口工作正常情况下的指示灯情况： 
	*a，连接状态指示灯呈绿色并且长亮，不亮表示未连接成功。 
	*b，不亮或者不闪烁表示无信号传输
这里设定：黄灯闪烁代表有数据传输，绿灯常亮代表与对端串口（网口）互联成功

******************************************/
#define	_LED_GLOBAL_

#include "Led.H"
#include <stdio.h>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "Public.h"




/*****************************************************************************
**Name:		 	LED_State_Ctl
**Function:	 	控制LED
**Args:		state :控制LED的开关
**Return:	None
******************************************************************************/
void LED_Key_Ctl(uchar state)
{
	IO_KEY_LED = state;
}

/*****************************************************************************
**Name:		 	LED_Toggle
**Function:	 	控制LED状态反转
**Args:		None
**Return:	None
******************************************************************************/
void LED_Key_Toggle(void)
{
	IO_KEY_LED = !IO_KEY_LED;
}

/*****************************************************************************
**Name:		 	LED_Double_Color
**Function:	 	控制双色灯的显示
**Args:		LDE Color
**Return:	None
******************************************************************************/
void LED_System_Ctl(uchar color)
{
	switch(color){
		case LED_W:
				IO_LED_R = SYSTEM_LED_OFF;	//无色（白色），灭
				IO_LED_B = SYSTEM_LED_OFF;			
			break;

		case LED_R:
				IO_LED_R = SYSTEM_LED_ON;	//红色
				IO_LED_B = SYSTEM_LED_OFF;
			break;

		case LED_B:
				IO_LED_R = SYSTEM_LED_OFF;	//可能是蓝色/绿色/黄色灯
				IO_LED_B = SYSTEM_LED_ON;
			break;
		
		case LED_Y:
				IO_LED_R = SYSTEM_LED_ON;	//混色,IO电平原因，红中带黄
				IO_LED_B = SYSTEM_LED_ON;
			break;
		
		default :
			break;		
	}
}

/*****************************************************************************
**Name:		 	LED_COM_Ctl
**Function:	 	控制LED，网口端的黄灯
**Args:		state :控制LED的开关
**Return:	None
******************************************************************************/
void LED_COM_Ctl(uchar state)
{
	IO_PM_LED1 = state;
}

/*****************************************************************************
**Name:		 	LED_LINK_Ctl
**Function:	 	控制LED，网口端的绿灯
**Args:		state :控制LED的开关
**Return:	None
******************************************************************************/
void LED_LINK_Ctl(uchar state)
{
	IO_PM_LED0 = state;
}

/*****************************************************************************
**Name:		 	LedTest
**Function:	 	led测试函数
**Args:		None
**Return:	None
******************************************************************************/
void LedTest(void)
{
	static ulong delayTmer;
	
	if(ReadUserTimer(&delayTmer) >= T_1S)
	{
		ResetUserTimer(&delayTmer);
		printf("Led Blink.\r\n");
	}
}

/*****************************************************************************
**Name:		 	LedDisplayPrg
**Function:	 	控制led的显示
**Args:		LedMode：控制的LED
			LedSta：控制的状态
**Return:	None
******************************************************************************/
void LedDisplayPrg(uchar LedMode, uchar LedSta)
{
	switch (LedMode) {
		case SYSTEM_LED:
			LED_System_Ctl(LedSta);
			break;
		case KEY_LED:
			LED_Key_Ctl(LedSta);
			break;
		case COM_LED:
			LED_COM_Ctl(LedSta);
			break;
		case LINK_LED:
			LED_LINK_Ctl(LedSta);
			break;
		default :
			break;
	}
}

/*****************************************************************************
**Name:		 	LedBlinkPrg
**Function:	 	控制led的显示
**Args:		None
**Return:	None
******************************************************************************/
void LedBlinkPrg(void)
{
	LedDisplayPrg(SYSTEM_LED, LED_R);
}


