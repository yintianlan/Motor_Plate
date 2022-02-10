//========================================================================
// 文件: STC_PWM.C
// 描述: PWM设置脉冲宽度 —— 任意周期和任意占空比DUTY%的PWM
// 版本: VER2.0
// 日期: 2021-12-7
// 备注: 
//	*初始化PWM及相关配置
//	*STC8H3KxxS2系列增强型PWM输出端口定义：
//	[PWM5:P2.3	PWM6:P1.6	PWM7:P1.7	PWM8:P1.7]
//	*所有与PWM相关的IO口，在上电后均为高阻输入态，必须将IO口设置为准双向或强推挽输出才能正常输出波形
//========================================================================
#define	_STC_PWM_GLOBAL_

#include <STC8H.h>
#include "STC8H_PWM.H"
#include "HardWare.H"
#include "GpioDefine.H"


/************************	功能说明	************************
    PWM_config();				//配置PWM

    PWM5_SetPwmWide(0);         //输出全低电平
    PWM5_SetPwmWide(1);         //输出1/CYCLE高电平
    PWM6_SetPwmWide(CYCLE);     //输出全高电平
    PWM6_SetPwmWide(CYCLE-1);   //输出(CYCLE-1)/CYCLE低电平
    PWM7_SetPwmWide(CYCLE/2);   //输出1/2高电平
    PWM7_SetPwmWide(CYCLE/3);   //输出1/3高电平
************************	功能说明	************************/


/*****************************************************************************/
/* IO口定义 */
sbit PWM5=P2^0;
sbit PWM6=P2^1;
sbit PWM7=P2^2;
sbit PWM8=P2^3;

/*****************************************************************************/
/* PWM 配置IO */
#define PWM5_1      0x00	//P2.0
#define PWM5_2      0x01	//P1.7
#define PWM5_3      0x02	//P0.0
#define PWM5_4      0x03	//P7.4

#define PWM6_1      0x00	//P2.1
#define PWM6_2      0x04	//P5.4
#define PWM6_3      0x08	//P0.1
#define PWM6_4      0x0C	//P7.5

#define PWM7_1      0x00	//P2.2
#define PWM7_2      0x10	//P3.3
#define PWM7_3      0x20	//P0.2
#define PWM7_4      0x30	//P7.6

#define PWM8_1      0x00	//P2.3
#define PWM8_2      0x40	//P3.4
#define PWM8_3      0x80	//P0.3
#define PWM8_4      0xC0	//P7.7

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40


//========================================================================
// 函数: void	PWM_config(void)
// 描述: PWM配置函数
// 参数: none.
// 返回: none.
//========================================================================
void PWM_config(void)
{	
    P2M0 &= ~0x0f;
    P2M1 &= ~0x0f;
    P2 &= ~0x0f;                    //设置P2.0/P2.1/P2.2/P2.3电平

	P_SW2 |= 0x80;					//使能访问XSFR，否则无法访问以下特殊寄存器
    PWMB_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMB_CCER2 = 0x00;
    PWMB_CCMR1 = 0x60; //通道模式配置,PWM模式1
    PWMB_CCMR2 = 0x60;
    PWMB_CCMR3 = 0x60;
    PWMB_CCMR4 = 0x60;

//    PWMB_CCER1 = 0x33; //配置通道5/6输出使能和极性（低电平有效）
//    PWMB_CCER2 = 0x33; //配置通道7/8输出使能和极性（低电平有效）
    PWMB_CCER1 = 0x11; //配置通道5/6输出使能和极性（高电平有效）
    PWMB_CCER2 = 0x11; //配置通道7/8输出使能和极性（高电平有效）
	
//	PWMB_PSCRH = 0x00;      //选择PWM的时钟为fCK_PSC/(PSCR[15:0]+1)
//	PWMB_PSCRL = 0x07;

	PWMB_ARR = CYCLE - 1;   //设置PWM周期,定义PWM周期计数为CYCLE

    PWMB_ENO = 0x00;
    PWMB_ENO |= ENO5P; //使能输出
    PWMB_ENO |= ENO6P; //使能输出
    PWMB_ENO |= ENO7P; //使能输出
    PWMB_ENO |= ENO8P; //使能输出

	PWMB_PS = 0x00;    //高级 PWM 通道输出脚选择位
    PWMB_PS |= PWM5_1; //选择 PWM5_1 通道
    PWMB_PS |= PWM6_1; //选择 PWM6_1 通道
    PWMB_PS |= PWM7_1; //选择 PWM7_1 通道
    PWMB_PS |= PWM8_1; //选择 PWM8_1 通道

    PWMB_BKR = 0x80;   //使能主输出
    PWMB_CR1 |= 0x01;  //开始计时
	P_SW2 &= 0x7f;					//关闭特殊功能寄存器XSFR
}


//========================================================================
// 函数: void PWM5_SetPwmWide(unsigned short Wide)
// 描述: PWM5配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM5_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
		PWMB_ENO &= ~ENO5P; //禁止输出
        PWM5 = 0;
    }
    else if (Wide >= (CYCLE - 1))
    {
        PWMB_ENO &= ~ENO5P; //禁止输出
        PWM5 = 1;
    }
    else
    {
        P_SW2 |= 0x80;      //使能访问PWM在扩展RAM区的特殊功能寄存器XSFR
		PWMB_CCR5 = Wide;	//设置占空比时间
        P_SW2 &= ~0x7f;
        PWMB_ENO |= ENO5P;	//使能PWM5信号输出
    }
}


//========================================================================
// 函数: void PWM6_SetPwmWide(unsigned short Wide)
// 描述: PWM6配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM6_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
		PWMB_ENO &= ~ENO6P; //禁止输出
        PWM6 = 0;
    }
    else if (Wide >= (CYCLE - 1))
    {
        PWMB_ENO &= ~ENO6P; //禁止输出
        PWM6 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
		PWMB_CCR6 = Wide;	//设置占空比时间
        P_SW2 &= ~0x7f;
        PWMB_ENO |= ENO6P;	//使能PWM6信号输出
    }
}


//========================================================================
// 函数: void PWM7_SetPwmWide(unsigned short Wide)
// 描述: PWM7配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM7_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
		PWMB_ENO &= ~ENO7P; //禁止输出
        PWM7 = 0;
    }
    else if (Wide >= (CYCLE - 1))
    {
        PWMB_ENO &= ~ENO7P; //禁止输出
        PWM7 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
		PWMB_CCR7 = Wide;	//设置占空比时间
        P_SW2 &= ~0x7f;
        PWMB_ENO |= ENO7P;	//使能PWM7信号输出
    }
}


//========================================================================
// 函数: void PWM8_SetPwmWide(unsigned short Wide)
// 描述: PWM8配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM8_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
		PWMB_ENO &= ~ENO8P; //禁止输出
        PWM8 = 0;
    }
    else if (Wide >= (CYCLE - 1))
    {
        PWMB_ENO &= ~ENO8P; //禁止输出
        PWM8 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
		PWMB_CCR8 = Wide;	//设置占空比时间
        P_SW2 &= ~0x7f;
        PWMB_ENO |= ENO8P;	//使能PWM8信号输出
    }
}

