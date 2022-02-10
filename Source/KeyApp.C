#define	_KEYAPP_GLOBAL_

#include <stdio.h>  

#include "HardWare.H"
#include "VariableCfg.H"

#include "KeyFunc.H"
#include "KeyApp.H"

#include "Delay.H"
#include "Public.h"
//#include "MotorIO.H"
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
**函数原型:  	void UpKeyHandle(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			上升按键 处理     				   
************************************************************************************************************************
*/
void UpKeyHandle(void)
{
	switch (KeyValue) {
		case 0x0101:			// 键按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x0201:			// 键弹起处理
			if(T_UP != currentTravel) {		//当前电机未到达最顶部，允许电机上升
				if(lifterState == M_DECLINING) {	//先让电机暂停一下，不然电机会抖动
					LifterDirSet(M_STOP);
					DelayMs(2);
				}

				LifterDirSet(M_RISING);
				printf("Motor Up\r\n");
			} else {
				printf("Arrive at the Top!\r\n");
			}
			
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		case 0x8001:			// 长按按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x7001:			// 长按弹起处理
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		default :
			break;
	}
}

/*
************************************************************************************************************************
**函数原型:  	void PauseKeyHandle(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			暂停按键 处理     				   
************************************************************************************************************************
*/
void PauseKeyHandle(void)
{
	switch (KeyValue) {
		case 0x0102:			// 键按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x0202:			// 键弹起处理
			LifterDirSet(M_STOP);
			printf("Motor Pause\r\n");
		
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
**函数原型:  	void DownKeyHandle(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			下降按键 处理     				   
************************************************************************************************************************
*/
void DownKeyHandle(void)
{
	switch (KeyValue) {
		case 0x0104:			// 键按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x0204:			// 键弹起处理
			if(T_DOWN != currentTravel) {		//当前电机未到达最底部，允许电机下降
				if(lifterState == M_RISING) {	//先让电机暂停一下，不然电机会抖动
					LifterDirSet(M_STOP);
					DelayMs(2);
				}

				LifterDirSet(M_DECLINING);
				printf("Motor Down\r\n");
			} else {
				printf("Arrive at the Bottom!\r\n");
			}
			
			LedDisplayPrg(KEY_LED, KEY_LED_OFF);	// 灭灯
			break;
		case 0x8004:			// 长按按下处理
			LedDisplayPrg(KEY_LED, KEY_LED_ON);
			break;
		case 0x7004:			// 长按弹起处理
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
			UpKeyHandle();
			break;
		/*************************** 控制2键处理 ********************************/
		case 0x0102:
		case 0x0202:				// 短按按下后弹起,
		case 0x8002:				// 长按按下，
		case 0x7002:				// 长按按下后弹起,
			PauseKeyHandle();
			break;
		/*************************** 控制3键处理 ********************************/
		case 0x0104:
		case 0x0204:				// 短按按下后弹起,
		case 0x8004:				// 长按按下，
		case 0x7004:				// 长按按下后弹起,
			DownKeyHandle();
			break;

		/****************************** 键处理 **********************************/
		default :
			//KeyHandle();
			break;
	}
}

/* end define	_KEYAPP_GLOBAL_*/
