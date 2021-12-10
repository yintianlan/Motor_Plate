#define	_KEYAPP_GLOBAL_

#include <stdio.h>  

#include "HardWare.H"
#include "VariableCfg.H"

#include "KeyFunc.H"
#include "KeyApp.H"

#include "Delay.H"
#include "STC8H_PWM.H"
#include "Public.h"
#include "MotorDirver.H"
#include "Led.H"


/*
************************************************************************************************************************
**函数原型:  	void KeyVarInit(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			该函数用于KEY模块的变量           				   
************************************************************************************************************************
*/
void KeyVarInit(void)
{
	KeyTypeFlag		= FALSE;
	LongKeyPush		= FALSE;
	KeyValue		= 0x00;
	KeyOprOkFlag	= FALSE;
	
	KeyLockStatus	= 0x00;			// 上电释放所有的按键
	KeyDelay10Ms	= 0x00;
	KeyPushPopFlg	= FALSE;
	
	KeyIndex		= 0x00;
	sKeyIndex		= 0x00;
	KeyKeepMs		= 0x00;
}


/*
************************************************************************************************************************
**函数原型:  	void CtlKeyHandle(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			电机控制1 Key 处理     				   
************************************************************************************************************************
*/
void CtlKeyHandle(void)
{
	switch (KeyValue) {
		case 0x0101:			// 键按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x0201:			// 键弹起处理
			mtRunCycle = mtRunCycle + (CYCLE/100)*10;//增加占空比
			if (mtRunCycle > CYCLE) {
				mtRunCycle = CYCLE;	//PWM直接输出全高电平
				printf("Warning: Motor Cycle Max!\r\n");
			}
			printf("Motor Cycle: %d\r\n", mtRunCycle);
			Motor_Cycle_Set(&sMotorFirst, mtRunCycle);
			Motor_Cycle_Set(&sMotorSecond, mtRunCycle);
			
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		case 0x8001:			// 长按按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x7001:			// 长按弹起处理
			mtRunCycle = 0;		//输出全低电平
			Motor_Cycle_Set(&sMotorFirst, mtRunCycle);
			Motor_Cycle_Set(&sMotorSecond, mtRunCycle);
			
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		default :
			break;
	}
}


/*
************************************************************************************************************************
**函数原型:  	void Ctl2KeyHandle(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			电机控制1 Key 处理     				   
************************************************************************************************************************
*/
void Ctl2KeyHandle(void)
{
	switch (KeyValue) {
		case 0x0102:			// 键按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x0202:			// 键弹起处理
			if (mtRunDir < 0x02) {
				mtRunDir++;
			} else {
				mtRunDir = 0;
			}		
			printf("Motor Dir: %02bX\r\n", mtRunDir);
			Motor_Dir_Set(&sMotorFirst, mtRunDir);
			Motor_Dir_Set(&sMotorSecond, mtRunDir);
		
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		case 0x8002:			// 长按按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x7002:			// 长按弹起处理
			
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		default :
			break;
	}
}

/*
************************************************************************************************************************
**函数原型:  	void KeyDealWithPrg(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			按键处理函数				   
************************************************************************************************************************
*/
void KeyDealWithPrg(void)
{	
	switch (KeyValue) {
		/*************************** 控制1按键处理 ********************************/
		case 0x0101:
		case 0x0201:				// 短按按下后弹起,
		case 0x8001:				// 长按按下，
		case 0x7001:				// 长按按下后弹起,
			CtlKeyHandle();
			break;
		/*************************** 控制2键处理 ********************************/
		case 0x0102:
		case 0x0202:				// 短按按下后弹起,
		case 0x8002:				// 长按按下，
		case 0x7002:				// 长按按下后弹起,
			Ctl2KeyHandle();
			break;		
		/*************************** 键处理 ********************************/
		case 0x0120:
		case 0x0220:				// 短按按下后弹起,
		case 0x8020:				// 长按按下，
		case 0x7020:				// 长按按下后弹起,
			//KeyHandle();
			break;
		/****************************** 键处理 **********************************/
		case 0x0240:				// 短按按下后弹起，
		case 0x8040:				// 长按按下，
			//KeyHandle();
			break;
		default :
			//KeyHandle();
			break;
	}
}

#endif	/* end define	_KEYAPP_GLOBAL_*/
