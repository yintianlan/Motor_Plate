#define	_VARIABLECFG_GLOBAL_

#include <STC8H.h>
#include <INTRINS.H>

#include <stdio.h>  
#include <string.h> 
#include <stdlib.h>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"

#include "Public.h"
#include "UartFunc.H"
#include "KeyFunc.H"
#include "KeyApp.H"
#include "TimeBase.H"
#include "Delay.H"
#include "MotorDirver.H"
#include "AT24C01_02.H"
#include "SerialApp.H"



void Wdt_Init(void);
void HardWareInit(void);
void SoftWareInit(void);
void SysPwrOnSetup(void);


void main(void)
{
//	DelayMs(500);						// 等待电源稳定
	GlobalInterCtl(0);					// Disable Global Interrupt
	HardWareInit();						// 系统硬件初始化
	SoftWareInit();						// 各外设/模块变量初始化
	SysPwrOnSetup();					// 系统上电初始化
	GlobalInterCtl(1);					// Enable Global Interrupt

	#if Debug == 1
	printf("-------------------------------\r\n");
	printf("app start.\r\n");
	LED_State_Ctl(LED_ON);				//快速闪一下提示灯
	DelayMs(200);
	LED_State_Ctl(LED_OFF);

	ee_Erase();
	AT24C02DebugPrg();					// AT24C02 Debug
	#endif
	
		
	while(1) {
		/************** 喂狗 **************************************/
		if (_testbit_(WdtClrEnaFlag)) {	// Fed Dog
			Wdt_Init();
		}

#if 1
		/************** 按键检测 **********************************/
		if (KeyLockStatus == 0x00) {	// 按键没有完全被锁定, 可以扫描
			KeyScanPrg();				// 功能键扫描
		}
		
		/************** 按键处理 **********************************/
		if (_testbit_(KeyOprOkFlag)) {				
			KeyDealWithPrg();
		}		

		/************** 处理串口接收帧 ****************************/
		if (_testbit_(SerialRcvOkFlag)) {			
			SerialFunctionPrg();
		}
				
		/************** LED控制 **********************************/
//		LedBlinkPrg();					// LED闪烁
#endif
		
	}
}

/*
************************************************************************************************************************
**函数原型:  	void Wdt_Init(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			看门狗初始化       				   
************************************************************************************************************************
*/
void Wdt_Init(void)
{
    WDT_CONTR = FED_DOG_VAL;//配置看门狗分频值，清除看门狗，启动看门狗

	WdtClrEnaFlag = FALSE;
	WdtClrCount	  = 0x00;
}

/*
************************************************************************************************************************
**函数原型:  	void HardWareInit(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			系统硬件初始化       				   
************************************************************************************************************************
*/
void HardWareInit(void)
{
	HAL_GPIO_Init();	//IO初始化
	MotorGpioInit();	//电机初始化
	Timer0Init();		// Timer0/1 Init		 		
	Timer1Init();
//	EN_485 = SEND_OFF;	// 禁止发送
	InitSerialBaudRate((ulong)(2400));		// Hard Serial Init
//	u1Init();//串口1初始化

	Wdt_Init();
}

/*
************************************************************************************************************************
**函数原型:  	void MainVarInit(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			主模块部分变量初始化       				   
************************************************************************************************************************
*/
void MainVarInit(void) 
{
}

/*
************************************************************************************************************************
**函数原型:  	void SoftWareInit(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			各个模块的变量初始化       				   
************************************************************************************************************************
*/
void SoftWareInit(void)
{
	MotorVarInit();			// Motor Variable Init
	KeyVarInit();			// Key Variable Init
	TimerVarInit();			// Timer Variable Init
	SerialVarInit();		// Serial Variable Init
	MainVarInit();
}

/*
************************************************************************************************************************
**函数原型:  	void SysPwrOnSetup(void)	                    
**参数说明:  	无
**返回值:    	无
**说明:			系统上电初始化 				   
************************************************************************************************************************
*/
void SysPwrOnSetup(void)
{
	uint id;
	uchar temp;

	//  --------------从24C02中读取 相关信息--------------------------------

	
	//  -------------- 补充功能 --------------------------------
	sMotorFirst.Mode = RUN;
	sMotorSecond.Mode = RUN;
	MotorEnCtl(&sMotorFirst);	//使能驱动电机IC
	MotorEnCtl(&sMotorSecond);

	mtRunDir = CW;
	Motor_Dir_Set(&sMotorFirst, mtRunDir);//控制电机转动方向
	Motor_Dir_Set(&sMotorSecond, mtRunDir);
}

#endif	/* end define	_VARIABLECFG_GLOBAL_*/
