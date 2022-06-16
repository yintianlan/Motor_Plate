//========================================================================
// 文件: Stepper_Motor.C
// 描述: PWM设置脉冲宽度 —— 任意周期和任意占空比DUTY%的PWM
// 版本: VER1.0
// 日期: 2021-1-7
// 作者: ANE-LQ
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


//========================================================================
//========================================================================

#if 0	
//2021-1-7 4相5线步进电机
//20BYJ46
//35BYJ412
//42YF22GN120S-TF0

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
bit initFlag = 0;


//电机初始化
void Motor_Init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	Coil_OFF;
	OS_Screen_BL_Set(XSP_12V_ON);
}

//延时函数
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
	
	static ulong tTimer = 0;
	
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
	Speed = 3; //调整速度
	
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

//步进电机转动测试
void Motor_Test(void)
{	
	//初始化电机
	if (0 == initFlag) {
		Motor_Init();
		initFlag = 1;
	}
	
	//电机驱动测试
	if(initFlag == 1) {
		//shun_fun_N();
		Coil_Test();
	}	
}

#endif

//========================================================================
//========================================================================
#if 0
//2021-2-10  2相4线步进电机
//CHW-GW4632-25BY
bit initFlag = 0;

//电机驱动使能-默认拉高
#define MT_Port P2	//电机端口

//电机输出通道
#define MtB 	P23		//B		Yellow
#define Mt_B 	P22		//_B_	Red
#define MtA 	P21		//A		Black
#define Mt_A 	P20		//_A_ 	Blue


#define Coil_Seen 		{MtB=1;Mt_B=0;MtA=1;Mt_A=0;}//A、B相通电，其他相断电
#define Coil_From 		{MtB=1;Mt_B=0;MtA=0;Mt_A=1;}//_A_、B相通电，其他相断电
#define Coil_Flang 		{MtB=0;Mt_B=1;MtA=0;Mt_A=1;}//_A_、_B_相通电，其他相断电
#define Coil_Side 		{MtB=0;Mt_B=1;MtA=1;Mt_A=0;}//A、_B_相通电，其他相断电

#define Coil_OFF 	{MtB=0;Mt_B=0;MtA=0;Mt_A=0;}//全部断电
#define Coil_ON 	{MtB=1;Mt_B=1;MtA=1;Mt_A=1;}//全部置1


//电机初始化
void Motor_Init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	Coil_OFF;
}

//延时函数
void Motor_Delay(int delayms)
{	
//	DelayMs(delayms);
	
	int i= 0;
	for(i = 0; i < delayms; i++) {
		Delay100us();
	}
}

//相位测试
//直接控制IO口，延时一段时间
void Coil_Test(void)
{
	unsigned char Speed;
	Speed = 15; //调整速度
	
	Coil_Seen;
	Motor_Delay(Speed);

	Coil_From;
	Motor_Delay(Speed);

	Coil_Flang;
	Motor_Delay(Speed);
	
	Coil_Side;                 //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
	Motor_Delay(Speed);      	//改变这个参数可以调整电机转速 ,
								//数字越小，转速越大,力矩越小
	
}

//步进电机转动测试
void Motor_Test(void)
{	
	//初始化电机
	if (0 == initFlag) {
		Motor_Init();
		initFlag = 1;
	}
	
	//电机驱动测试
	if(initFlag == 1) {
		Coil_Test();
	}	
}
#endif


//========================================================================
//========================================================================
#if 1
//2022-2-10 
//36JXFS30K3.7G80,35减速箱
//该型号电机无规格书，测试发现每一相位至少0.7ms以上才能启动，且延时超过1.2ms就会出现丢步现象

//20HD3401-05,丝杆电机
//该型号电机无参数，测试发现每一相位至少0.7ms以上才能启动，且延时超过1.2ms就会出现丢步现象

//35HD2401-05,丝杆电机
//该型号电机无规格书，测试发现每一相位至少2ms以上才能启动，且延时超过1.2ms就会出现丢步现象

//35HD2407-05
//该型号电机步进角为1.8°，1.2A
//如果需要转动 1 圈，那么需要 360°/1.8° = 200 个脉冲信号。

//28HD3401-8
//该型号电机步进角为1.8°，0.67A


bit initFlag = 0;
static ulong tTimer = 0;


//电机驱动使能-默认拉高
#define MT_Port P2	//电机端口

//电机输出通道
////36JXFS30K3.7G80
//#define Mt_B 	P23		//_B_	YEL
//#define MtB 	P22		//B		RED
//#define Mt_A 	P21		//_A_	ORG
//#define MtA 	P20		//A 	BRN

////20HD3401-05
////28HD3401-8
//#define Mt_B 	P23		//_B_	BLU
//#define MtB 	P22		//B		RED
//#define Mt_A 	P21		//_A_	GRN
//#define MtA 	P20		//A 	BLK


//35HD2401-05
//35HD2407-05
#define Mt_B 	P23		//_B_	BLU
#define Mt_A 	P22		//_A_	GRN
#define MtB 	P21		//B		YEL
#define MtA 	P20		//A 	RED


#define Coil_Seen 		{MtA=1;MtB=1;Mt_A=0;Mt_B=0;}//A、B相通电，其他相断电
#define Coil_From 		{MtA=0;MtB=1;Mt_A=1;Mt_B=0;}//_A_、B相通电，其他相断电
#define Coil_Flang 		{MtA=0;MtB=0;Mt_A=1;Mt_B=1;}//_A_、_B_相通电，其他相断电
#define Coil_Side 		{MtA=1;MtB=0;Mt_A=0;Mt_B=1;}//A、_B_相通电，其他相断电

#define Coil_OFF 	{MtA=0;MtB=0;Mt_A=0;Mt_B=0;}//全部断电
#define Coil_ON 	{MtA=1;MtB=1;Mt_A=1;Mt_B=1;}//全部置1


//电机初始化
void Motor_Init(void)
{
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平
	
	Coil_OFF;
}

//延时函数
void Motor_Delay(int delayms)
{	
	DelayMs(delayms);
	
//	int i= 0;
//	for(i = 0; i < delayms; i++) {
//		Delay100us();
//	}
}

//相位测试，转动一个步进角
//直接控制IO口，延时一段时间
void Coil_Test(void)
{
	unsigned char Speed;
	Speed = 1; //调整速度
	
	Coil_Seen;
	Motor_Delay(Speed);

	Coil_From;
	Motor_Delay(Speed);

	Coil_Flang;
	Motor_Delay(Speed);
	
	Coil_Side;                 //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
	Motor_Delay(Speed);      	//改变这个参数可以调整电机转速 ,
								//数字越小，转速越大,力矩越小
	
//	Coil_OFF;
//	DelayMs(500);		//每转动一个步距角，延时一下
}

//相位测试，转动一个步进角
//直接控制IO口，延时一段时间
void Coil_Test_1(void)
{
	unsigned char Speed;
	Speed = 1; //调整速度

	Coil_Side;                 //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
	Motor_Delay(Speed);      	//改变这个参数可以调整电机转速 ,
								//数字越小，转速越大,力矩越小
	Coil_Flang;
	Motor_Delay(Speed);

	Coil_From;
	Motor_Delay(Speed);
	
	Coil_Seen;
	Motor_Delay(Speed);
}

//顺转一圈
void mt_foreward(void)
{
	unsigned int i;
	for(i = 0; i < 200; i++)		//N步，为实际步数
	{
		Coil_Test();
	}

	Coil_OFF;
//	DelayMs(1000);		//每转动一圈，延时一下
}

//反转一圈
void mt_rollback(void)
{
	unsigned int i;
	for(i = 0; i < 200; i++)		//N步，为实际步数
	{
		Coil_Test_1();
	}

	Coil_OFF;
//	DelayMs(500);		//每转动一圈，延时一下
}


void shun_fun_N(void)
{
	static bit direcFlag = 1; // 初始状态为正向

	
	if( (direcFlag == 0))
	{
		mt_foreward();
		if(ReadUserTimer(&tTimer) >= 1000 * 5) {
			direcFlag = 1;
			ResetUserTimer(&tTimer);
		}
	}
	
	if( (direcFlag == 1))
	{
		mt_rollback();
		if(ReadUserTimer(&tTimer) >= 1000 * 5) {
			direcFlag = 0;
			ResetUserTimer(&tTimer);
		}
	}	
}

//步进电机转动测试
void Motor_Test(void)
{	
	//初始化电机
	if (0 == initFlag) {
		Motor_Init();
		initFlag = 1;
		ResetUserTimer(&tTimer);
	}
	
	//电机驱动测试
	if(initFlag == 1) {
		shun_fun_N();
		
		//Coil_Test();
		//Coil_Test_1();
	}	
}
#endif
