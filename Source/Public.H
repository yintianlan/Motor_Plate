#ifndef ___PUBLIC_H__
#define ___PUBLIC_H__

#include "Typedef.h"


/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _PUBLIC_GLOBAL_
	#define	PUBLIC_GLOBAL	extern
#else
	#define	PUBLIC_GLOBAL
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*
************************************************************************************
* 						函数定义
************************************************************************************
*/
PUBLIC_GLOBAL	void HAL_GPIO_Init(void);
PUBLIC_GLOBAL	void LED_State_Ctl(uchar state);
PUBLIC_GLOBAL	void LED_Toggle(void);

PUBLIC_GLOBAL	void ResetUserTimer(ulong *Timer);
PUBLIC_GLOBAL	ulong ReadUserTimer(ulong *Timer);


#endif
	