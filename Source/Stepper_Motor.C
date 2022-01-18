#include <STC8H.h>
#include <INTRINS.H>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"

#include "Stepper_Motor.H"
#include "Delay.H"
#include "Public.h"


//电机驱动使能-默认拉高
//电机端口
#define MT_Port P2

//电机输出通道
#define Mt1 P23		//_B_
#define Mt2 P22		//_A_
#define Mt3 P21		//B
#define Mt4 P20		//A


uchar code pulseTable0[] = {0x08, 0x04, 0x02, 0x01, 0x01, 0x02, 0x04, 0x08}; // 一相励磁(同一时刻只有一个线圈通电，旋转角1.8度)
uchar code pulseTable1[] = {0x0c, 0x06, 0x03, 0x09, 0x09, 0x03, 0x06, 0x0c}; // 二相励磁(同一时刻有两个线圈通电，旋转角1.8度)
uchar code pulseTable2[] = {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09,
						    0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08}; // 一-二相励磁场(一二相交替励磁，旋转角0.9度)

//表格，换算成二进制1000，1100，0100，0110，0010，0011，0001，1001
//假设P2口输出低4位驱动电机	A, B, _A_, _B_
//1-2相励磁，发8次脉冲（顺序输出下面表格）转动4步
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

#define		RUN_STRP	(1000)
#define 	Factor 5 // 转速控制常数
uchar speed = 0, startPos = 0; // 默认正转
bit oper = 0/*操作数*/, direcFlag = 0; // 初始状态为正向

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


//顺转200步
void shun_N(void)
{
	static unsigned int i;
	unsigned char n;
	n = 0;
	for(i = 0; i < RUN_STRP; i++)		//200步，i/2为实际步数
	{
		MT_Port = BiaoGe[n]|0xF0;	//不影响P2其它IO口输出
		Motor_Delay(3);		
		n = n + 1;
		if(n > 7) n = 0;
	}
	MT_Port &= 0xF0;			//保证电机绕组断电
}

//反转200步
void fun_N(void)
{
	static unsigned int i;
	unsigned char n;
	n = 8;
	for(i = 0; i < RUN_STRP; i++)		//200步，i/2为实际步数
	{
		n = n - 1;
		MT_Port = BiaoGe[n]|0xF0;	//不影响P2其它IO口输出
		Motor_Delay(3);
		if(n == 0) n = 8;
	}
	MT_Port &= 0xF0;			//保证电机绕组断电
}

//正反转测试
//正转200步（180°），等1s，反转200步，等1s
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
