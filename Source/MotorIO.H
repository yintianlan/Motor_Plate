#ifndef _MOTOR_IO_H_
#define	_MOTOR_IO_H_

#include "Typedef.h"
#include "VariableCfg.H"


/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _MOTOR_GLOBAL_
	#define	MOTOR_GLOBAL	extern
#else
	#define	MOTOR_GLOBAL
#endif
	
	

/*
********************************************************************************************************************
* 						宏定义
********************************************************************************************************************
*/


MOTOR_GLOBAL	void MotorGpioInit(void);
MOTOR_GLOBAL	void MotorVarInit(void);
	
	
MOTOR_GLOBAL	void MotorRunCtl(structMotorUnitInfo *motor);
MOTOR_GLOBAL	void MotorEnCtl(structMotorUnitInfo *motor);

void  Motor_Dir_Set(structMotorUnitInfo *motor, uchar dir);
void  Motor_Cycle_Set(structMotorUnitInfo *motor, uint cycle);
void  Motor_First_Ctl(structMotorUnitInfo *motor);
void  Motor_Second_Ctl(structMotorUnitInfo *motor);
	
#endif
