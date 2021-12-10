#ifndef __LED_H__
#define __LED_H__

#include "Typedef.h"


/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _LED_GLOBAL_
	#define	LED_GLOBAL	extern
#else
	#define	LED_GLOBAL
#endif


/*
************************************************************************************
* 						函数定义
************************************************************************************
*/
LED_GLOBAL	void LED_Key_Ctl(uchar state);
LED_GLOBAL	void LED_Key_Toggle(void);
LED_GLOBAL	void LED_System_Ctl(uchar color);
LED_GLOBAL	void LED_COM_Ctl(uchar state);
LED_GLOBAL	void LED_LINK_Ctl(uchar state);
	
LED_GLOBAL	void LedDisplayPrg(uchar LedMode, uchar LedSta);
LED_GLOBAL	void LedBlinkPrg(void);

#endif
