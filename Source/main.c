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
#include "AT24C01_02.H"
#include "SerialApp.H"
#include "Led.H"
#include "ADC.H"
#include "TravelSwitch.H"
#include "MotorDirver.H"
#include "Stepper_Motor.H"



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
	LedDisplayPrg(SYSTEM_LED, LED_R);				//快速闪一下提示灯
	DelayMs(200);
	LedDisplayPrg(SYSTEM_LED, LED_W);

	ee_Erase();
	AT24C02DebugPrg();					// AT24C02 Debug
	#endif
		
	while(1) {
		/************** 喂狗 **************************************/
		if (_testbit_(WdtClrEnaFlag)) {	// Fed Dog
			Wdt_Init();
		}
		
		//Coil_Test();
		//shun_fun_N();
		step_motor_drive(1,5);
		
#if 0
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
		LedBlinkPrg();					// LED闪烁
		
		/************** ADC电压采样 *******************************/
//		Read_VoltageValue();			// 读取电压值
		
		/************** 外部信号检测 *******************************/
		TravelSwitchPrg();				//行程开关		
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
//	MotorGpioInit();	//电机初始化
	step_motor_init();
	Timer0Init();		// Timer0 Init		 		
//	Timer1Init();		// Timer1 Init
	IO_RS485_EN = SEND_OFF;	// 禁止发送
	InitSerialBaudRate((ulong)(115200));		// Hard Serial Init
	ADC_Init();		// ADC Init
	
//	Wdt_Init();	
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
	TravelVarInit();		//Travel Switch Variable Init
//	MotorVarInit();			// Motor Variable Init
	KeyVarInit();			// Key Variable Init
	TimerVarInit();			// Timer Variable Init
	SerialVarInit();		// Serial Variable Init
	ADCVarInit();			//ADC参数初始化
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

	//  -------------- 关闭LED --------------------------------
	LedDisplayPrg(SYSTEM_LED, LED_W);
	LedDisplayPrg(KEY_LED, KEY_LED_OFF);
	LedDisplayPrg(COM_LED, COM_LED_OFF);
	LedDisplayPrg(LINK_LED, LINK_LED_OFF);

	//  --------------从24C02中读取 相关信息--------------------------------

	

	
	//  -------------- 补充功能 --------------------------------
//	LifterDirSet(M_STOP);
}

#endif	/* end define	_VARIABLECFG_GLOBAL_*/
