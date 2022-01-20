/*
********************************************************************************************************************
*描述：    	Display Function SoftWare
*文件名:   	KeyFunc.C
*应用语言: 	KEIL C51
*版本 :    	V1.0
*作者：   	宁雪玉
********************************************************************************************************************
*/
/*************  功能说明    **************
通过读取AD值判断按下的按键

方法暂时不支持判断同时按下多个按键（组合按键）

ADC读取会有误差，注意添加误差值

******************************************/
#define	_KEYFUNC_GLOBAL_

#include "KeyFunc.H"
#include "GpioDefine.H"
#include "HardWare.H"
#include "VariableCfg.H"
#include "ADC.H"

#include <stdio.h>  

#define		ADC_ERROR_RANGE		10	// 误差允许值0.10V
#define		ADC_KEY_NONE		500	// 无按键时的电压5.00V，以实际为准

#define		ADC_KEY1_VAL		460	// 按键1按下时的电压值
#define		ADC_KEY2_VAL		435	// 按键2按下时的电压值
#define		ADC_KEY3_VAL		375	// 按键3按下时的电压值


/*
************************************************************************************************************************
**函数原型:  	uchar KeyReadPrg(void)                     
**参数说明:  	无
**返回值:    	无
**说明:			Read Key Value          				   
************************************************************************************************************************
*/
static uchar KeyReadPrg(void)
{
	uchar temp = 0x00;
	uint  value = 0x00;

	value = Get_VoltageValue(ADC9);//读取ADC9的电压值	
	
	if(((ADC_KEY_NONE + ADC_ERROR_RANGE) > value) && \
		((ADC_KEY_NONE - ADC_ERROR_RANGE)) < value ) 
	{
		temp = 0x00;	//无按键
	} 
	else if(((ADC_KEY1_VAL + ADC_ERROR_RANGE) > value) && \
		(ADC_KEY1_VAL - ADC_ERROR_RANGE) < value ) 
	{
		temp |= (1 << 0);	//S1
	}
	else if(((ADC_KEY2_VAL + ADC_ERROR_RANGE) > value) && \
		(ADC_KEY2_VAL - ADC_ERROR_RANGE) < value ) 
	{
		temp |= (1 << 1);	//S2
	}
	else if(((ADC_KEY3_VAL + ADC_ERROR_RANGE) > value) && \
		(ADC_KEY3_VAL - ADC_ERROR_RANGE) < value ) 
	{
		temp |= (1 << 2);	//S3
	}
	
	return (temp);
}	

/*
************************************************************************************************************************
**函数原型:  	void KeyScanPrg(void)                     
**参数说明:  	无
**返回值:    	无
**说明:			Scan Key Value          				   
************************************************************************************************************************
*/
void KeyScanPrg(void)
{
	static uint KeyScanStatus = 0;
//	static uchar KeyDelayTime = 0;
	
	static uint KeyValuePre = 0;
	if (KeyValuePre != KeyValue) {
		printf("machine KeyValue: 0x%04x\r\n", KeyValue);
		KeyValuePre = KeyValue;
	}
	
	switch (KeyScanStatus) {
		case 0:
			if (KeyReadPrg() != 0x00) {				// 有键按下
				KeyScanStatus++;
				KeyKeepMs = 15;					// 消除抖动时间,NMS
			}
			else {
				KeyScanStatus	= 0;
				KeyKeepMs	= 0;
				KeyValue = 0x0000;					//初始状态按键值
			}
			break;
		case 1:
			if (KeyReadPrg() != 0x00) {				// 消除抖动
				if (KeyKeepMs == 0) {
					KeyScanStatus++;
				}
			}
			else {									// 出现了抖动
				KeyScanStatus	= 0;
				KeyKeepMs	= 0;
			}
			break;
		case 2:
			if (KeyReadPrg() != 0x00) {
				sKeyIndex = KeyReadPrg();	// 按键按下，但是未弹起
				KeyScanStatus++;
				KeyKeepMs = 5;
			}
			else {
				KeyScanStatus	= 0;
				KeyKeepMs	= 0;
			}
			break;
		case 3:
			if (KeyReadPrg() != 0x00) {				// 消除抖动
				if (KeyKeepMs == 0) {
					KeyScanStatus++;
				}
			}
			else {
				KeyScanStatus	= 0;
				KeyKeepMs	= 0;
			}
			break;
		case 4:
			if (KeyReadPrg() != 0x00) {				// 读取按键值
				KeyIndex = KeyReadPrg();					// 按键按下，但是未弹起
				if (KeyIndex == sKeyIndex) {		// 第一次采样的值与当前采样的值相等
					if (KeyPushPopFlg == FALSE)	{		// 当前按键是第一次按下已经弹起
						KeyPushPopFlg = TRUE;				// 则再次按下
						
						KeyValue = 0x0100 + KeyReadPrg();	// 按键按下，但是未弹起
						KeyOprOkFlag 	= TRUE;				// 按键按下成功
					}
					else {
						KeyValue = 0x0200 + KeyReadPrg();	// 按键松开值
					}
					KeyScanStatus++;
					KeyTypeFlag = FALSE;					// 短按
					KeyDelay10Ms = 0;
				}
				else {
					sKeyIndex = KeyIndex;			// 更新按键编号值
					KeyScanStatus	= 3;
					KeyKeepMs 	= 10;			// 消除抖动时间,NMS
				}
			}
			else {									// 出现抖动
				KeyScanStatus	= 0;
				KeyKeepMs 	= 0;
			}
			break;
		case 5:	
			if (KeyReadPrg() == 0x00) {				// 松开按键
				KeyScanStatus++;
				KeyKeepMs = 5;
			}
			else {
				if (KeyDelay10Ms > 150) {			// 1.5S
					KeyScanStatus = 7;
					KeyValue = 0x8000 + KeyReadPrg();
					KeyTypeFlag = TRUE;	// 长按键
				}	
			}
			break;
		case 6:										// 松开消除抖动
			if (KeyReadPrg() == 0x00) {
				if (KeyKeepMs == 0) {
					KeyScanStatus++;
				}
			}
			else {									// 还没有弹起
				KeyScanStatus--;
			}
			break;
		case 7:
			KeyOprOkFlag 	= TRUE;
			if (KeyTypeFlag == TRUE) {		// 长按
				KeyScanStatus++;
			}
			else {									// 短按
				KeyPushPopFlg = FALSE;	// 短按弹起
				KeyValue &= 0x00FF;		// 按键松开值
				KeyValue |= 0x0200;
				KeyScanStatus 	= 0;
				KeyKeepMs  	= 0;
			}
			break;
		case 8:
			if (KeyReadPrg() == 0x00) {
				KeyScanStatus++;
				KeyKeepMs = 6;
			}
			break;
		case 9:
			if (KeyReadPrg() == 0x00) {
				if (KeyKeepMs == 0) {
					KeyTypeFlag  	= FALSE;
					KeyPushPopFlg = FALSE;
					KeyScanStatus 	= 0;
					KeyKeepMs  	= 0;

					KeyValue &= 0x00FF;
					KeyValue |= 0x7000;
					KeyOprOkFlag= TRUE;
				}
			}
			else {
				KeyScanStatus--;
			}
			break;
		default :
			KeyOprOkFlag 	= FALSE;
			KeyScanStatus 	= 0;
			KeyKeepMs  	= 0;
			break;
	}
}

#endif	/* end define	_KEYFUNC_GLOBAL_*/
