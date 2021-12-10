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


/* IO口定义 */
sbit PWM0=P3^7;
sbit PWM1=P2^1;
sbit PWM2=P3^7;
sbit PWM3=P2^1;
sbit PWM4=P2^2;
sbit PWM5=P2^3;
sbit PWM6=P1^6;
sbit PWM7=P1^7;

/* PWM 配置寄存器 */
//sfr     P_SW2       =   0xba;
sfr     PWMCFG      =   0xf1;
sfr     PWMIF       =   0xf6;
sfr     PWMFDCR     =   0xf7;
sfr     PWMCR       =   0xfe;

/* PWM 各通道寄存器 */
#define PWMC            (*(unsigned int volatile xdata *)0xfff0)
#define PWMCKS          (*(unsigned char volatile xdata *)0xfff2)
#define TADCP           (*(unsigned int volatile xdata *)0xfff3)
#define PWM0T1          (*(unsigned int volatile xdata *)0xff00)
#define PWM0T2          (*(unsigned int volatile xdata *)0xff02)
#define PWM0CR          (*(unsigned char volatile xdata *)0xff04)
#define PWM0HLD         (*(unsigned char volatile xdata *)0xff05)
#define PWM1T1          (*(unsigned int volatile xdata *)0xff10)
#define PWM1T2          (*(unsigned int volatile xdata *)0xff12)
#define PWM1CR          (*(unsigned char volatile xdata *)0xff14)
#define PWM1HLD         (*(unsigned char volatile xdata *)0xff15)
#define PWM2T1          (*(unsigned int volatile xdata *)0xff20)
#define PWM2T2          (*(unsigned int volatile xdata *)0xff22)
#define PWM2CR          (*(unsigned char volatile xdata *)0xff24)
#define PWM2HLD         (*(unsigned char volatile xdata *)0xff25)
#define PWM3T1          (*(unsigned int volatile xdata *)0xff30)
#define PWM3T2          (*(unsigned int volatile xdata *)0xff32)
#define PWM3CR          (*(unsigned char volatile xdata *)0xff34)
#define PWM3HLD         (*(unsigned char volatile xdata *)0xff35)
#define PWM4T1          (*(unsigned int volatile xdata *)0xff40)
#define PWM4T2          (*(unsigned int volatile xdata *)0xff42)
#define PWM4CR          (*(unsigned char volatile xdata *)0xff44)
#define PWM4HLD         (*(unsigned char volatile xdata *)0xff45)
#define PWM5T1          (*(unsigned int volatile xdata *)0xff50)
#define PWM5T2          (*(unsigned int volatile xdata *)0xff52)
#define PWM5CR          (*(unsigned char volatile xdata *)0xff54)
#define PWM5HLD         (*(unsigned char volatile xdata *)0xff55)
#define PWM6T1          (*(unsigned int volatile xdata *)0xff60)
#define PWM6T2          (*(unsigned int volatile xdata *)0xff62)
#define PWM6CR          (*(unsigned char volatile xdata *)0xff64)
#define PWM6HLD         (*(unsigned char volatile xdata *)0xff65)
#define PWM7T1          (*(unsigned int volatile xdata *)0xff70)
#define PWM7T2          (*(unsigned int volatile xdata *)0xff72)
#define PWM7CR          (*(unsigned char volatile xdata *)0xff74)
#define PWM7HLD         (*(unsigned char volatile xdata *)0xff75)


/************************	功能说明	************************
    PWM_config();				//配置PWM

    PWM2_SetPwmWide(0);         //输出全低电平
    PWM3_SetPwmWide(1);         //输出1/2550高电平
    PWM4_SetPwmWide(CYCLE);     //输出全高电平
    PWM5_SetPwmWide(CYCLE-1);   //输出2549/2550低电平
    PWM6_SetPwmWide(CYCLE/2);   //输出1/2高电平
    PWM7_SetPwmWide(CYCLE/3);   //输出1/3高电平
************************	功能说明	************************/



//========================================================================
// 函数: void	PWM_config(void)
// 描述: PWM配置函数
// 参数: none.
// 返回: none.
//========================================================================
void PWM_config(void)
{
//    P1M0 &= ~0xc0;
//    P1M1 &= ~0xc0;
//    P1 &= ~0xc0;                    //设置P1.6/.P1.7电平

    P1M0 &= ~0x40;
    P1M1 &= ~0x40;
    P1 &= ~0x40;                    //设置P1.6电平	
	
    P2M0 &= ~0x0e;
    P2M1 &= ~0x0e;
    P2 &= ~0x0e;                    //设置P2.1/P2.2/P2.3电平
//    P3M0 &= ~0x80;
//    P3M1 &= ~0x80;
//    P3 &= ~0x80;                    //设置P3.7电平
	
	P_SW2 |= 0x80;					//使能访问XSFR，否则无法访问以下特殊寄存器
	PWMCFG = 0x00;                  //配置PWM的输出初始电平为低电平，也就是第一次翻转前输出的电平
	PWMCKS = 0x0f;                  //选择PWM的时钟为Fosc/(PS[3:0]+1)
	PWMC = CYCLE;                   //设置PWM周期,定义PWM周期(最大值为32767)

//	PWM2T1 = 1;               		//设置PWM2第1次反转的PWM计数,也就是电平第一次发生翻转的计数值 
//	PWM2T2 = 0;						//设置PWM2第2次反转的PWM计数,PWM2T1与PWM2T2不能设置为相等的值，否则产生竞争
//	PWM2CR = 0x00;                  //选择PWM2输出到P3.7,关闭PWM2中断   

	PWM3T1 = 1;
	PWM3T2 = 0;
	PWM3CR = 0x00;                  //选择PWM3输出到P2.1,关闭PWM3中断

	PWM4T1 = 1;
	PWM4T2 = 0;         
	PWM4CR = 0x00;                  //选择PWM4输出到P2.2,关闭PWM4中断

	PWM5T1 = 1;
	PWM5T2 = 0;
	PWM5CR = 0x00;                  //选择PWM4输出到P2.3,关闭PWM5中断

	PWM6T1 = 1;
	PWM6T2 = 0;
	PWM6CR = 0x00;                  //选择PWM4输出到P1.6,关闭PWM6中断

//	PWM7T1 = 1;
//	PWM7T2 = 0;
//	PWM7CR = 0x00;                  //选择PWM4输出到P1.7,关闭PWM7中断
	PWMCR |= 0x80;                  //使能PWM模块
	P_SW2 &=~0x80;					//关闭特殊功能寄存器XSFR
}


//void pwm2_set_duty(unsigned int DUTY)           //PWM2
//{
//	if (DUTY==0)
//	{
//		PWMCR &=~0x01;
//		PWM2=0;
//	}
//	else if (DUTY==100)
//	{
//		PWMCR &=~0x01;
//		PWM2=1;
//	}
//	else
//	{
//		P_SW2 |= 0x80;                  //使能访问PWM在扩展RAM区的特殊功能寄存器XSFR
//		PWM2T1 = 0x0001;                //设置PWM2第1次反转的PWM计数
//		PWM2T2 = CYCLE * DUTY / 100;    //设置PWM2第2次反转的PWM计数
//		P_SW2 &=~ 0x80;                 //占空比为(PWM2T2-PWM2T1)/PWMC
//		PWMCR |= 0x01;                  //使能PWM信号输出
//	}	
//}


//========================================================================
// 函数: void PWM2_SetPwmWide(unsigned int Wide)
// 描述: PWM2配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM2_SetPwmWide(unsigned int Wide)           //PWM2
{
    if (Wide == 0)
    {
        PWMCR &= ~0x01;
        PWM2 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x01;
        PWM2 = 1;
    }
    else
    {
        P_SW2 |= 0x80;                  //使能访问PWM在扩展RAM区的特殊功能寄存器XSFR
        PWM2T1 = Wide;                	//设置PWM2第1次反转的PWM计数，占空比为(PWM2T2-PWM2T1)/PWMC
        P_SW2 &= ~0x80;                 //关闭特殊功能寄存器XSFR
        PWMCR |= 0x01;                  //使能PWM2信号输出
    }
}

//========================================================================
// 函数: void PWM3_SetPwmWide(unsigned short Wide)
// 描述: PWM3配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM3_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
        PWMCR &= ~0x02;
        PWM3 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x02;
        PWM3 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM3T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x02;                  //使能PWM3信号输出
    }
}


//========================================================================
// 函数: void PWM4_SetPwmWide(unsigned short Wide)
// 描述: PWM4配置函数
// 参数: Wide:反转计数
// 返回: none.
//========================================================================
void PWM4_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
        PWMCR &= ~0x04;
        PWM4 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x04;
        PWM4 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM4T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x04;                  //使能PWM4信号输出
    }
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
        PWMCR &= ~0x08;
        PWM5 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x08;
        PWM5 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM5T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x08;                  //使能PWM5信号输出
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
        PWMCR &= ~0x10;
        PWM6 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x10;
        PWM6 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM6T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x10;                  //使能PWM6信号输出
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
        PWMCR &= ~0x20;
        PWM7 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x20;
        PWM7 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM7T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x20;                  //使能PWM7信号输出
    }
}

#endif
