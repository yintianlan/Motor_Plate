//========================================================================
// 文件: Stepper_Motor.C
// 描述: PWM设置脉冲宽度 —— 任意周期和任意占空比DUTY%的PWM
// 版本: VER1.0
// 日期: 2021-1-7
// 备注: 
//1、步进电机必须加驱动才可以运转，驱动信号必须为脉冲信号，
//	 没有脉冲的时候，步进电机静止， 如果加入适当的脉冲信号， 
//	 就会以一定的角度（称为步角）转动。转动的速度和脉冲的频率成正比。
//
//2、20BYJ46 12V 驱动的 4 相 5 线的步进电机，而且是减速步进电机，
//	 减速比为 1:85，步进角为 7.5/85 度。如果需要转动 1 圈，
//	 那么需要 360/7.5*85 = 4080 个脉冲信号。
//
//3、步进电机具有瞬间启动和急速停止的优越特性。
//
//4、改变脉冲的顺序， 可以方便的改变转动的方向。
//========================================================================


#include <STC8H.h>
#include <INTRINS.H>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"

#include "Stepper_Motor.H"
#include "Delay.H"
#include "Public.h"


#if 0
#define STEP_MOTOR_A_PIN          P20     /*红色*/
#define STEP_MOTOR_B_PIN          P21     /*橙色*/
#define STEP_MOTOR_C_PIN          P22     /*黄色*/
#define STEP_MOTOR_D_PIN          P23     /*粉色*/

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
static void step_motor_a_output(void);
static void step_motor_ab_output(void);
static void step_motor_b_output(void);
static void step_motor_bc_output(void);
static void step_motor_c_output(void);
static void step_motor_cd_output(void);
static void step_motor_d_output(void);
static void step_motor_da_output(void);
static void step_motor_foreward_output(uint8 step);
static void step_motor_rollback_output(uint8 step);
static void step_motor_foreward(uint8 speed);
static void step_motor_rollback(uint8 speed);

const control_opt_t control_processors[] = {
		{ 0, step_motor_a_output },
		{ 1, step_motor_ab_output },
		{ 2, step_motor_b_output },
		{ 3, step_motor_bc_output },
		{ 4, step_motor_c_output },
		{ 5, step_motor_cd_output },
		{ 6, step_motor_d_output },
		{ 7, step_motor_da_output },
};

/*电机相位转动步数计数*/
static uint8 MotorStepCount = 0;

//========================================================================
// 简述: 步进电机驱动延时函数
// 参数: 无
// 返回: 无
// 详述: 
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
void step_motor_init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	step_motor_stop();				//电机停止
	OS_Screen_BL_Set(XSP_12V_ON);	//COM端的12V供电脚
	
}

//========================================================================
// 简述: 步进电机驱动
// 参数: dire：步进电机方向   speed：步进电机转速
// 返回: 无
// 详述: 
//========================================================================
void step_motor_drive(uint8 dire, uint8 speed)
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
// 注意事项: 步进电机停止后需要使四个相位引脚都为高电平，否则步进电机会发热。
// 			 因为步进电机公共端为高电平，所有引脚都为高电平就不会产生电流，就不会发热。
//========================================================================
void step_motor_stop(void)
{
	STEP_MOTOR_A_PIN = LEVEL_H;
	STEP_MOTOR_B_PIN = LEVEL_H;
	STEP_MOTOR_C_PIN = LEVEL_H;
	STEP_MOTOR_D_PIN = LEVEL_H;
}

//========================================================================
// 简述: 步进电机正转
// 参数: 无
// 返回: 无
// 详述: 顺时针转
//========================================================================
static void step_motor_foreward(uint8 speed)
{ 
	if((MotorStepCount++) > 7)
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
	if((MotorStepCount++) > 7)
		MotorStepCount = 0;
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
	
//   switch(step)
//	 {
//		 case 0:
//		 {
//			  step_motor_a_output();
//		    break;
//		 }
//		 case 1:
//		 {
//			  step_motor_ab_output();
//		    break;
//		 }
//		 case 2:
//		 {
//			  step_motor_b_output();
//		    break;
//		 }
//		 case 3:
//		 {
//			  step_motor_bc_output();
//		    break;
//		 }
//		 case 4:
//		 {
//			  step_motor_c_output();
//		    break;
//		 }
//		 case 5:
//		 {
//			  step_motor_cd_output();
//		    break;
//		 }
//		 case 6:
//		 {
//			  step_motor_d_output();
//		    break;
//		 }
//		 case 7:
//		 {
//			  step_motor_da_output();
//		    break;
//		 }
//		 default:
//			  break;
//	 }
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

//   switch(step)
//	 {
//		 case 0:
//		 {
//			  step_motor_a_output();
//		    break;
//		 }
//		 case 1:
//		 {
//			  step_motor_da_output();
//		    break;
//		 }
//		 case 2:
//		 {
//			  step_motor_d_output();
//		    break;
//		 }
//		 case 3:
//		 {
//			  step_motor_cd_output();
//		    break;
//		 }
//		 case 4:
//		 {
//			  step_motor_c_output();
//		    break;
//		 }
//		 case 5:
//		 {
//			  step_motor_bc_output();
//		    break;
//		 }
//		 case 6:
//		 {
//			  step_motor_b_output();
//		    break;
//		 }
//		 case 7:
//		 {
//			  step_motor_ab_output();
//		    break;
//		 }
//		 default:
//			  break;
//	 }
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机A相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_a_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_H;
	STEP_MOTOR_B_PIN = LEVEL_L;
	STEP_MOTOR_C_PIN = LEVEL_L;
	STEP_MOTOR_D_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机AB相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_ab_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_H;
	STEP_MOTOR_B_PIN = LEVEL_H;
	STEP_MOTOR_C_PIN = LEVEL_L;
	STEP_MOTOR_D_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机B相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_b_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_L;
	STEP_MOTOR_B_PIN = LEVEL_H;
	STEP_MOTOR_C_PIN = LEVEL_L;
	STEP_MOTOR_D_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机BC相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_bc_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_L;
	STEP_MOTOR_B_PIN = LEVEL_H;
	STEP_MOTOR_C_PIN = LEVEL_H;
	STEP_MOTOR_D_PIN = LEVEL_L;
}
//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机C相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_c_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_L;
	STEP_MOTOR_B_PIN = LEVEL_L;
	STEP_MOTOR_C_PIN = LEVEL_H;
	STEP_MOTOR_D_PIN = LEVEL_L;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机CD相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_cd_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_L;
	STEP_MOTOR_B_PIN = LEVEL_L;
	STEP_MOTOR_C_PIN = LEVEL_H;
	STEP_MOTOR_D_PIN = LEVEL_H;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机D相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_d_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_L;
	STEP_MOTOR_B_PIN = LEVEL_L;
	STEP_MOTOR_C_PIN = LEVEL_L;
	STEP_MOTOR_D_PIN = LEVEL_H;
}

//========================================================================
// 简述: 步进电机引脚控制
// 参数: 无
// 返回: 无
// 详述: 步进电机DA相输出（ULN2003有反向功能）
//========================================================================
static void step_motor_da_output(void)
{
	STEP_MOTOR_A_PIN = LEVEL_H;
	STEP_MOTOR_B_PIN = LEVEL_L;
	STEP_MOTOR_C_PIN = LEVEL_L;
	STEP_MOTOR_D_PIN = LEVEL_H;
}
#endif



#if 1
//电机驱动使能-默认拉高
#define MT_Port P2	//电机端口

//电机输出通道
#define Mt1 P23		//_B_
#define Mt2 P22		//_A_
#define Mt3 P21		//B
#define Mt4 P20		//A


uchar code pulseTable0[] = {0x08, 0x04, 0x02, 0x01, 0x01, 0x02, 0x04, 0x08}; // 一相励磁(同一时刻只有一个线圈通电，旋转角1.8度)
uchar code pulseTable1[] = {0x0c, 0x06, 0x03, 0x09, 0x09, 0x03, 0x06, 0x0c}; // 二相励磁(同一时刻有两个线圈通电，旋转角1.8度)
uchar code pulseTable2[] = {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09,
						    0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08}; // 一-二相励磁场(一二相交替励磁，旋转角0.9度)

////表格，换算成二进制1000，1100，0100，0110，0010，0011，0001，1001
////假设P2口输出低4位驱动电机	A, B, _A_, _B_
////1-2相励磁，发8次脉冲（顺序输出下面表格）转动4步
//unsigned char code BiaoGe[8] = {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};
							
//表格，换算成二进制0001，0011，0010，0110，0100，1100，1000，1001
//假设P2口输出低4位驱动电机	A, B, _A_, _B_
//1-2相励磁，发8次脉冲（顺序输出下面表格）转动4步
unsigned char code BiaoGe[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};



#define Coil_A1 	{Mt4=1;Mt3=0;Mt2=0;Mt1=0;}//A相通电，其他相断电
#define Coil_AB 	{Mt4=1;Mt3=1;Mt2=0;Mt1=0;}//A相通电，其他相断电
#define Coil_B1 	{Mt4=0;Mt3=1;Mt2=0;Mt1=0;}//B相通电，其他相断电
#define Coil_BC 	{Mt4=0;Mt3=1;Mt2=1;Mt1=0;}//B相通电，其他相断电
#define Coil_C1 	{Mt4=0;Mt3=0;Mt2=1;Mt1=0;}//C相通电，其他相断电
#define Coil_CD 	{Mt4=0;Mt3=0;Mt2=1;Mt1=1;}//C相通电，其他相断电
#define Coil_D1 	{Mt4=0;Mt3=0;Mt2=0;Mt1=1;}//D相通电，其他相断电
#define Coil_DA 	{Mt4=1;Mt3=0;Mt2=0;Mt1=1;}//D相通电，其他相断电
#define Coil_OFF 	{Mt4=0;Mt3=0;Mt2=0;Mt1=0;}//全部断电
#define Coil_ON 	{Mt4=1;Mt3=1;Mt2=1;Mt1=1;}//全部置1

#define		RUN_STEP	(4080)		//转动一圈需要的步数
#define 	Factor 5 // 转速控制常数
uchar speed = 0, startPos = 0; // 默认正转
bit oper = 0;		/*操作数*/
bit direcFlag = 0; // 初始状态为正向

//电机初始化
void Motor_Init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	Coil_OFF;
	OS_Screen_BL_Set(XSP_12V_ON);
}

void Motor_Delay(int delayms)
{	
	DelayMs(delayms);
}


//顺转N步
void shun_N(void)
{
	unsigned int i;
	unsigned char n;
	n = 0;
	for(i = 0; i < RUN_STEP; i++)		//N步，N/2为实际步数
	{
		MT_Port = BiaoGe[n]|0xF0;	//不影响P2其它IO口输出
		Motor_Delay(2);		
		n = n + 1;
		if(n > 7) n = 0;
	}
	MT_Port &= 0xF0;			//保证电机绕组断电
}

//反转N步
void fun_N(void)
{
	unsigned int i;
	unsigned char n;
	n = 8;
	for(i = 0; i < RUN_STEP; i++)		//N步，N/2为实际步数
	{
		n = n - 1;
		MT_Port = BiaoGe[n]|0xF0;	//不影响P2其它IO口输出
		Motor_Delay(2);
		if(n == 0) n = 8;
	}
	MT_Port &= 0xF0;			//保证电机绕组断电
}

//正反转测试
//正转N步（360°），等1s，反转N步，等1s
void shun_fun_N(void)
{
//	shun_N();
//	DelayMs(1000);
//	fun_N();
//	DelayMs(1000);
	
	static ulong tTimer;
	
	if( (direcFlag == 0) && (ReadUserTimer(&tTimer) >= 1000))
	{
		shun_N();
		direcFlag = 1;
		ResetUserTimer(&tTimer);
	}
	
	if( (direcFlag == 1) && (ReadUserTimer(&tTimer) >= 1000))
	{
		fun_N();
		direcFlag = 0;
		ResetUserTimer(&tTimer);
	}	
}

//相位测试
//直接控制IO口，延时一段时间
void Coil_Test(void)
{
	unsigned char Speed;
	Speed = 5; //调整速度
	
	Coil_A1;                 //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
	Motor_Delay(Speed);         //改变这个参数可以调整电机转速 ,
							//数字越小，转速越大,力矩越小
	Coil_AB;
	Motor_Delay(Speed);
	Coil_B1;
	Motor_Delay(Speed);
	Coil_BC;
	Motor_Delay(Speed);
	Coil_C1;
	Motor_Delay(Speed);
	Coil_CD;
	Motor_Delay(Speed);
	Coil_D1;
	Motor_Delay(Speed);	
	Coil_DA;
	Motor_Delay(Speed);

//	Coil_OFF;
//	DelayMs(Speed);

//	Coil_ON;
//	DelayMs(Speed);
}

/*步进电机控制执行函数*/
//direcFlag 控制方向:0 正向，1 方向
//Factor 电机转速
void execute(void)
{
	uchar i, j;
	startPos = (direcFlag == 0) ? 0 : 4; // 方向控制
	for(i = startPos; i <= (startPos + 4); i++){
		MT_Port = pulseTable0[i]|0xF0;
		for(j = 0; j < (speed + 1) * Factor; j++){ // 用延时来控制脉冲输出的频率，从而控制步进电机转速
			DelayMs(10);
		} 
	}
}
#endif

