#define	_MOTOR_GLOBAL_

#include "MotorIO.H"

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"



/*******************************************************************************
  * @函数名称	void MotorGpioInit(void)
  * @函数说明		电机IO初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorGpioInit(void)
{
	//电机驱动默认使能，供电为12V
	//IO上电输出全低
	IO_MT1=0;		//输出全低电平
	IO_MT2=0;		//输出全低电平

	IO_MT3=0;		//输出全低电平
	IO_MT4=0;		//输出全低电平
}

/*******************************************************************************
  * @函数名称	void MotorVarInit(void)
  * @函数说明		电机相关变量初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorVarInit(void)
{
	mtRunCycle = MT_CYCLE_MAX;
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
  * @函数说明		端接电机1转动方向/转速控制
  * @输入参数   motor：电机信息
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  Motor_First_Ctl(structMotorUnitInfo *motor) //端接电机1控制程序
{
	if(motor->Out != M12) return;	//选择电机输出1,2
	
	switch(motor->Dir) {
		case CW:	//顺时针转动
			IO_MT1=1;
			IO_MT2=0;    	//输出全低电平
			break;
		
		case CCW:	//逆时针转动
			IO_MT1=0;   	//输出全低电平
			IO_MT2=1;
			break;
		
		case STOP:	//电机停止
			IO_MT1=0;		//输出全低电平
			IO_MT2=0;		//输出全低电平
			break;
		
		case BREAK:	//刹车
			IO_MT1=1;	//输出全高电平
			IO_MT2=1;	//输出全高电平
			break;

		default:
			break;	
	}
}

/*******************************************************************************
  * @函数名称	void Motor_Second_Ctl(structMotorUnitInfo *motor)
  * @函数说明		端接电机2转动方向/转速控制
  * @输入参数   motor：电机信息
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  Motor_Second_Ctl(structMotorUnitInfo *motor) //端接电机2控制程序
{
	if(motor->Out != M34) return;	//选择电机输出3,4
	
	switch(motor->Dir) {
		case CW:	//顺时针转动
			IO_MT3=1;
			IO_MT4=0;    	//输出全低电平
			break;
		
		case CCW:	//逆时针转动
			IO_MT3=0;   	//输出全低电平
			IO_MT4=1;
			break;
		
		case STOP:	//电机停止
			IO_MT3=0;		//输出全低电平
			IO_MT4=0;		//输出全低电平
			break;
		
		case BREAK:	//刹车
			IO_MT3=1;	//输出全高电平
			IO_MT4=1;	//输出全高电平
			break;

		default:
			break;	
	}
}

/*******************************************************************************
  * @函数名称	void MotorVarInit(void)
  * @函数说明		电机转动方向控制
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MotorRunCtl(structMotorUnitInfo *motor)
{
	if(motor->Out == M12)//选择电机输出1,2
	{
		if(motor->Dir == STOP) {		//电机停止
			IO_MT1=1;
			IO_MT2=1;
		} else if (motor->Dir == CCW) {	//逆时针方向
			IO_MT1=0;
			IO_MT2=1;
		} else  if (motor->Dir == CW){	//顺时针方向
			IO_MT1=1;
			IO_MT2=0;
		}
	}
	else if(motor->Out == M34)//选择电机输出3,4
	{
		if(motor->Dir == STOP) {		//电机停止
			IO_MT3=1;     //输出全高电平
			IO_MT4=1;
		} else if (motor->Dir == CCW) {	//逆时针方向
			IO_MT3=0;
			IO_MT4=1;
		} else  if (motor->Dir == CW){	//顺时针方向
			IO_MT3=1;
			IO_MT4=0;
		}	
	}
}



#endif
