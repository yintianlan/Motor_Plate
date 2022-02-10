//========================================================================
// 文件: GW4632_25BY.C
// 描述: 使用IO口控制步进电机 —— CHW-GW4632-25BY
// 版本: VER1.0
// 日期: 2021-2-10
// 备注: 
//1、步进电机必须加驱动才可以运转，驱动信号必须为脉冲信号，
//	 没有脉冲的时候，步进电机静止， 如果加入适当的脉冲信号， 
//	 就会以一定的角度（称为步角）转动。转动的速度和脉冲的频率成正比。
//
//2、CHW-GW4632-25BY 12V 驱动的 2 相 4 线的步进电机，而且是减速步进电机，
//	 减速比最大为 1:1019，步进角为 0.015 度。如果需要转动 1 圈，
//	 那么需要 360/0.015 ≈ 24,000 个脉冲信号。
//
//3、步进电机具有瞬间启动和急速停止的优越特性。
//
//4、改变脉冲的顺序， 可以方便的改变转动的方向。
//
//5、CHW-GW4632-25BY 空载牵入频率：>=800PPS，在裸机运行状态，
//	 即没有任何外加负载的时候，从静止到转动的状态，能够成功启动步进电机而且不丢步的最高频率为800hz，1.25ms
//	 空载牵出频率：>=1300PPS，在已经启动的状态下，不断地增加频率，
//   直到达到步进电机的极限，步进电机开始呈现出抖动且出现丢步运行，此时的频率1300hz，0.769ms
//========================================================================


#include <STC8H.h>
#include <INTRINS.H>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"

#include "GW4632_25BY.H"
#include "Delay.H"
#include "Public.h"


#define STEP_MOTOR_B_PIN          P23     /*黄色*/
#define STEP_MOTOR_B1_PIN         P22     /*红色*/
#define STEP_MOTOR_A_PIN          P21     /*黑色*/
#define STEP_MOTOR_A1_PIN         P20     /*蓝色*/

#define STEP_MOTOR_SPEED          2		/*步进电机的控制速率*/

#define STEP_MOTOR_DIRE_FOREWARD  1     /*步进电机正向转：顺时针*/
#define STEP_MOTOR_DIRE_ROLLBACK  2     /*步进电机正向转：逆时针*/

#define	LEVEL_H		1		/*相位电平输出高，这里1为高，ULN2003可能有反向功能*/
#define	LEVEL_L		0		/*相位电平输出低*/


typedef struct {
	uint8 cmd;
	void (*processor)(void);
} control_opt_t;

/*函数声明*/
static void step_motor_seen_output(void);
static void step_motor_from_output(void);
static void step_motor_flang_output(void);
static void step_motor_side_output(void);
static void step_motor_foreward_output(uint8 step);
static void step_motor_rollback_output(uint8 step);
static void step_motor_foreward(uint8 speed);
static void step_motor_rollback(uint8 speed);

static const control_opt_t control_processors[] = {
		{ 0, step_motor_side_output },
		{ 1, step_motor_flang_output },
		{ 2, step_motor_from_output },
		{ 3, step_motor_seen_output },
};

/*电机相位转动步数计数*/
static uint8 MotorStepCount = 0;

//========================================================================
// 简述: 步进电机驱动延时函数
// 参数: 无
// 返回: 无
// 详述: 注意步进电机的启动频率，需要控制节拍刷新时间大于1/f,
//		 否则电机不能正常启动，可能发生丢步或堵转。
//========================================================================
static void step_motor_delay(int delayms)
{	
	DelayMs(delayms);
}

//========================================================================
// 简述: 步进电机驱动初始化
// 参数: 无
// 返回: 无
// 详述: 
//========================================================================
void chw_motor_init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	chw_motor_stop();				//电机停止
}

//========================================================================
// 简述: 步进电机驱动
// 参数: dire：步进电机方向   speed：步进电机转速
// 返回: 无
// 详述: 
//========================================================================
void chw_motor_drive(uint8 dire, uint8 speed)
{
   switch(dire)
	 {
		 case STEP_MOTOR_DIRE_FOREWARD:
		 {
			  step_motor_foreward(speed);
		    break;
		 }
		 case STEP_MOTOR_DIRE_ROLLBACK:
		 {
			  step_motor_rollback(speed);
		    break;
		 }
		 default:
			  break;
	 }
}

//========================================================================
// 简述: 步进电机停止
// 参数: 无
// 返回: 无
// 详述: （ULN2003有反向功能）
// 注意事项: 步进电机停止后需要使两个相位引脚都为低电平，否则步进电机会发热。
//========================================================================
void chw_motor_stop(void)
{
	STEP_MOTOR_B_PIN  = LEVEL_L;
	STEP_MOTOR_B1_PIN = LEVEL_L;
	STEP_MOTOR_A_PIN  = LEVEL_L;
	STEP_MOTOR_A1_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机正转
// 参数: 无
// 返回: 无
// 详述: 顺时针转
//========================================================================
static void step_motor_foreward(uint8 speed)
{ 
	if((MotorStepCount++) > 3)
		MotorStepCount = 0;
	step_motor_foreward_output(MotorStepCount);
	step_motor_delay(speed);
}

//========================================================================
// 简述: 步进电机反转
// 参数: 无
// 返回: 无
// 详述: 逆时针转
//========================================================================
static void step_motor_rollback(uint8 speed)
{
	if(MotorStepCount == 0)
		MotorStepCount = 3;
	else
		MotorStepCount--;
	step_motor_rollback_output(MotorStepCount);
	step_motor_delay(speed);
}

//========================================================================
// 简述: 步进电机正向输出
// 参数: 无
// 返回: 无
// 详述: 
//========================================================================
static void step_motor_foreward_output(uint8 step)
{
	control_processors[step].processor();	//使用函数指针的方式
}

//========================================================================
// 简述: 步进电机反向输出
// 参数: 无
// 返回: 无
// 详述: 
//========================================================================
static void step_motor_rollback_output(uint8 step)
{
	control_processors[step].processor();	//使用函数指针的方式
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机A相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_seen_output(void)
{
	STEP_MOTOR_B_PIN  = LEVEL_H;
	STEP_MOTOR_B1_PIN = LEVEL_L;
	STEP_MOTOR_A_PIN  = LEVEL_H;
	STEP_MOTOR_A1_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机B相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_from_output(void)
{
	STEP_MOTOR_B_PIN  = LEVEL_H;
	STEP_MOTOR_B1_PIN = LEVEL_L;
	STEP_MOTOR_A_PIN  = LEVEL_L;
	STEP_MOTOR_A1_PIN = LEVEL_H;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机C相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_flang_output(void)
{
	STEP_MOTOR_B_PIN  = LEVEL_L;
	STEP_MOTOR_B1_PIN = LEVEL_H;
	STEP_MOTOR_A_PIN  = LEVEL_L;
	STEP_MOTOR_A1_PIN = LEVEL_H;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机D相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_side_output(void)
{
	STEP_MOTOR_B_PIN  = LEVEL_L;
	STEP_MOTOR_B1_PIN = LEVEL_H;
	STEP_MOTOR_A_PIN  = LEVEL_H;
	STEP_MOTOR_A1_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机转动测试
// 参数: 无
// 返回: 无
// 详述: 
//========================================================================
void chw_motor_test(void)
{
	static uint8 initFlag = 0;
	
	//初始化电机
	if(initFlag != 1) {
		chw_motor_init();
		initFlag = 1;
	}
	
	//电机驱动测试
	if(initFlag) {
		chw_motor_drive(STEP_MOTOR_DIRE_FOREWARD, STEP_MOTOR_SPEED);
	}
}

