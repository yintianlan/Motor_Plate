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
//	DelayMs(500);						// �ȴ���Դ�ȶ�
	GlobalInterCtl(0);					// Disable Global Interrupt
	HardWareInit();						// ϵͳӲ����ʼ��
	SoftWareInit();						// ������/ģ�������ʼ��
	SysPwrOnSetup();					// ϵͳ�ϵ��ʼ��
	GlobalInterCtl(1);					// Enable Global Interrupt

	#if Debug == 1
	printf("-------------------------------\r\n");
	printf("app start.\r\n");
	LedDisplayPrg(SYSTEM_LED, LED_R);				//������һ����ʾ��
	DelayMs(200);
	LedDisplayPrg(SYSTEM_LED, LED_W);

	ee_Erase();
	AT24C02DebugPrg();					// AT24C02 Debug
	#endif
		
	while(1) {
		/************** ι�� **************************************/
		if (_testbit_(WdtClrEnaFlag)) {	// Fed Dog
			Wdt_Init();
		}
		
		//Coil_Test();
		//shun_fun_N();
		step_motor_drive(1,5);
		
#if 0
		/************** ������� **********************************/
		if (KeyLockStatus == 0x00) {	// ����û����ȫ������, ����ɨ��
			KeyScanPrg();				// ���ܼ�ɨ��
		}
		
		/************** �������� **********************************/
		if (_testbit_(KeyOprOkFlag)) {				
			KeyDealWithPrg();
		}		

		/************** �����ڽ���֡ ****************************/
		if (_testbit_(SerialRcvOkFlag)) {			
			SerialFunctionPrg();
		}
				
		/************** LED���� **********************************/
		LedBlinkPrg();					// LED��˸
		
		/************** ADC��ѹ���� *******************************/
//		Read_VoltageValue();			// ��ȡ��ѹֵ
		
		/************** �ⲿ�źż�� *******************************/
		TravelSwitchPrg();				//�г̿���		
#endif
		
	}
}


/*
************************************************************************************************************************
**����ԭ��:  	void Wdt_Init(void)	                    
**����˵��:  	��
**����ֵ:    	��
**˵��:			���Ź���ʼ��       				   
************************************************************************************************************************
*/
void Wdt_Init(void)
{
	WDT_CONTR = FED_DOG_VAL;//���ÿ��Ź���Ƶֵ��������Ź����������Ź�

	WdtClrEnaFlag = FALSE;
	WdtClrCount	  = 0x00;
}

/*
************************************************************************************************************************
**����ԭ��:  	void HardWareInit(void)	                    
**����˵��:  	��
**����ֵ:    	��
**˵��:			ϵͳӲ����ʼ��       				   
************************************************************************************************************************
*/
void HardWareInit(void)
{
	HAL_GPIO_Init();	//IO��ʼ��
//	MotorGpioInit();	//�����ʼ��
	step_motor_init();
	Timer0Init();		// Timer0 Init		 		
//	Timer1Init();		// Timer1 Init
	IO_RS485_EN = SEND_OFF;	// ��ֹ����
	InitSerialBaudRate((ulong)(115200));		// Hard Serial Init
	ADC_Init();		// ADC Init
	
//	Wdt_Init();	
}

/*
************************************************************************************************************************
**����ԭ��:  	void MainVarInit(void)	                    
**����˵��:  	��
**����ֵ:    	��
**˵��:			��ģ�鲿�ֱ�����ʼ��       				   
************************************************************************************************************************
*/
void MainVarInit(void) 
{
}

/*
************************************************************************************************************************
**����ԭ��:  	void SoftWareInit(void)	                    
**����˵��:  	��
**����ֵ:    	��
**˵��:			����ģ��ı�����ʼ��       				   
************************************************************************************************************************
*/
void SoftWareInit(void)
{
	TravelVarInit();		//Travel Switch Variable Init
//	MotorVarInit();			// Motor Variable Init
	KeyVarInit();			// Key Variable Init
	TimerVarInit();			// Timer Variable Init
	SerialVarInit();		// Serial Variable Init
	ADCVarInit();			//ADC������ʼ��
	MainVarInit();
}

/*
************************************************************************************************************************
**����ԭ��:  	void SysPwrOnSetup(void)	                    
**����˵��:  	��
**����ֵ:    	��
**˵��:			ϵͳ�ϵ��ʼ�� 				   
************************************************************************************************************************
*/
void SysPwrOnSetup(void)
{
	uint id;
	uchar temp;

	//  -------------- �ر�LED --------------------------------
	LedDisplayPrg(SYSTEM_LED, LED_W);
	LedDisplayPrg(KEY_LED, KEY_LED_OFF);
	LedDisplayPrg(COM_LED, COM_LED_OFF);
	LedDisplayPrg(LINK_LED, LINK_LED_OFF);

	//  --------------��24C02�ж�ȡ �����Ϣ--------------------------------

	

	
	//  -------------- ���书�� --------------------------------
//	LifterDirSet(M_STOP);
}

#endif	/* end define	_VARIABLECFG_GLOBAL_*/
