#define	_MOTOR_GLOBAL_

#include "MotorDirver.H"

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "STC8H_PWM.H"



/*******************************************************************************
  * @函数名称	void MotorGpioInit(void)
  * @函数说明	电机IO初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorGpioInit(void)
{
	//电机驱动默认使能，供电为12V
    PWM_config();	//配置PWM	
}

/*******************************************************************************
  * @函数名称	void MotorVarInit(void)
  * @函数说明	电机相关变量初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorVarInit(void)
{
	mtRunCycle = MOTOR_CYCLE_5;
	mtRunTimCnt = 0x00;
	mtRunDir = STOP;
	
	sMotorFirst.EnDes = MT;			//初始化端接电机1的相关变量
	sMotorFirst.Out = M12;
	sMotorFirst.Dir = mtRunDir;
	sMotorFirst.Mode = FREE;
	sMotorFirst.Cycle = mtRunCycle;	//PWM的占空比 = mtRunCycle/CYCLE，两个电机需一致
	
	sMotorSecond.EnDes = MT;		//初始化端接电机2的相关变量
	sMotorSecond.Out = M34;
	sMotorSecond.Dir = mtRunDir;
	sMotorSecond.Mode = FREE;
	sMotorSecond.Cycle = mtRunCycle;
	
	lifterState = M_STOP;
}


/*******************************************************************************
  * @函数名称	LifterDirSet
  * @函数说明	升降器方向控制
  * @输入参数  	dir：方向
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void LifterDirSet(uchar dir)
{
	lifterState = dir;//升降器方向设置
	MotorDirSet(lifterState);
}

/*******************************************************************************
  * @函数名称	MotorDirSet
  * @函数说明	升降器电机转向控制
  * @输入参数   dir：方向
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorDirSet(uchar dir)
{
	switch(dir) {
		case M_STOP:	//停止
				Motor_Dir_Set(&sMotorFirst, STOP);//控制电机转动方向
				Motor_Dir_Set(&sMotorSecond, STOP);
			break;
		
		case M_RISING:	//上升
				Motor_Dir_Set(&sMotorFirst, CW);
				Motor_Dir_Set(&sMotorSecond, CW);
			break;
		
		case M_DECLINING:	//下降
				Motor_Dir_Set(&sMotorFirst, CCW);
				Motor_Dir_Set(&sMotorSecond, CCW);
			break;

		default:
			break;
	}
}

/*******************************************************************************
  * @函数名称	void Motor_Dir_Set
  * @函数说明		电机转向控制
  * @输入参数   motor：电机信息
				dir：方向
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Motor_Dir_Set(structMotorUnitInfo *motor, uchar dir)
{
	motor->Dir = dir;		//加载参数
	
	if(motor->Out == M12) {	//选择电机输出1,2
		Motor_First_Ctl(motor);
	} else {
		Motor_Second_Ctl(motor);
	}
}

/*******************************************************************************
  * @函数名称	void Motor_Cycle_Set
  * @函数说明		电机转速控制
  * @输入参数   motor：电机信息
				cycle：输出周期
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Motor_Cycle_Set(structMotorUnitInfo *motor, uint cycle) //加速/减速
{
	motor->Cycle = cycle;	//加载参数
	
	if(motor->Out == M12) {	//选择电机输出1,2
		Motor_First_Ctl(motor);
	} else {
		Motor_Second_Ctl(motor);
	}
}

/*******************************************************************************
  * @函数名称	void Motor_First_Ctl(structMotorUnitInfo *motor)
  * @函数说明	端接电机1转动方向/转速控制
  * @输入参数   motor：电机信息
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  Motor_First_Ctl(structMotorUnitInfo *motor) //端接电机1控制程序
{
	if(motor->Out != M12) return;	//选择电机输出1,2
	
	switch(motor->Dir) {
		case CW:	//顺时针转动
			PWM8_SetPwmWide(motor->Cycle);
			PWM7_SetPwmWide(0);    	//输出全低电平
			break;
		
		case CCW:	//逆时针转动
			PWM8_SetPwmWide(0);   	//输出全低电平
			PWM7_SetPwmWide(motor->Cycle);
			break;
		
		case STOP:	//电机停止
			PWM8_SetPwmWide(0);		//输出全低电平
			PWM7_SetPwmWide(0);		//输出全低电平
			break;
		
		case BREAK:	//刹车
			PWM8_SetPwmWide(CYCLE);	//输出全高电平
			PWM7_SetPwmWide(CYCLE);	//输出全高电平
			break;

		default:
			break;	
	}
}

/*******************************************************************************
  * @函数名称	void Motor_Second_Ctl(structMotorUnitInfo *motor)
  * @函数说明	端接电机2转动方向/转速控制
  * @输入参数   motor：电机信息
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  Motor_Second_Ctl(structMotorUnitInfo *motor) //端接电机2控制程序
{
	if(motor->Out != M34) return;	//选择电机输出3,4
	
	switch(motor->Dir) {
		case CW:	//顺时针转动
			PWM6_SetPwmWide(motor->Cycle);
			PWM5_SetPwmWide(0);    	//输出全低电平
			break;
		
		case CCW:	//逆时针转动
			PWM6_SetPwmWide(0);   	//输出全低电平
			PWM5_SetPwmWide(motor->Cycle);
			break;
		
		case STOP:	//电机停止
			PWM6_SetPwmWide(0);		//输出全低电平
			PWM5_SetPwmWide(0);		//输出全低电平
			break;
		
		case BREAK:	//刹车
			PWM6_SetPwmWide(CYCLE);	//输出全高电平
			PWM5_SetPwmWide(CYCLE);	//输出全高电平
			break;

		default:
			break;	
	}
}

/*******************************************************************************
  * @函数名称	void MotorVarInit(void)
  * @函数说明	电机转动方向控制
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorRunCtl(structMotorUnitInfo *motor)
{
	if(motor->Out == M12)//选择电机输出1,2
	{
		if(motor->Dir == STOP) {		//电机停止
//			IO_MT1=1;
			PWM8_SetPwmWide(CYCLE);     //输出全高电平
			IO_MT2=1;
		} else if (motor->Dir == CCW) {	//逆时针方向
//			IO_MT1=0;
			PWM8_SetPwmWide(motor->Cycle);
			IO_MT2=1;
		} else  if (motor->Dir == CW){	//顺时针方向
//			IO_MT1=1;
			PWM8_SetPwmWide(motor->Cycle);
			IO_MT2=0;
		}
	}
	else if(motor->Out == M34)//选择电机输出3,4
	{
		if(motor->Dir == STOP) {		//电机停止
//			IO_MT3=1;
			PWM6_SetPwmWide(CYCLE);     //输出全高电平
			IO_MT4=1;
		} else if (motor->Dir == CCW) {	//逆时针方向
//			IO_MT3=0;
			PWM6_SetPwmWide(motor->Cycle);
			IO_MT4=1;
		} else  if (motor->Dir == CW){	//顺时针方向
//			IO_MT3=1;
			PWM6_SetPwmWide(motor->Cycle);
			IO_MT4=0;
		}	
	}
}

/*******************************************************************************
  * @函数名称	void MotorEnCtl(uchar mtEn, uchar mode)
  * @函数说明	电机状态控制
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorEnCtl(structMotorUnitInfo *motor)
{
	switch(motor->EnDes) {
		case MT: 	//默认电机，直接供电，不需要控制
			if(motor->Mode == FREE) {
			} else {
			}
			break;		
		
//		case MA: 	//电机A
//			if(motor->Mode == FREE) {
//				MtA = 0;
//			} else {
//				MtA = 1;
//			}
//			break;
//		
//		case MB: 	//电机B
//			if(motor->Mode == FREE) {
//				MtB = 0;
//			} else {
//				MtB = 1;
//			}
//			break;

//		case MC: 	//电机C
//			if(motor->Mode == FREE) {
//				MtC = 0;
//			} else {
//				MtC = 1;
//			}
//			break;

//		case MD: 	//电机D
//			if(motor->Mode == FREE) {
//				MtD = 0;
//			} else {
//				MtD = 1;
//			}
//			break;

		default:
			break;
	}
}


#endif
