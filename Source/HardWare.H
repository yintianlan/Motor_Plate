/*
********************************************************************************************************************
*描述：    	硬件资源利用头文件定义
*文件名:   	HardWare.h
*应用语言: 	KEIL C51
*版本 :    	V1.0
*作者:		ANE-LQ
********************************************************************************************************************
*/ 
#ifndef _HARDWAREH_H_
#define	_HARDWAREH_H_

/*
********************************************************************************************************************
* 						头文件
********************************************************************************************************************
*/
#include <STC8H.h>
#include <INTRINS.H>

/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _HARDWAREH_GLOBAL_
	#define	HARDWAREH_GLOBAL	extern
#else
	#define	HARDWAREH_GLOBAL
#endif

/*
********************************************************************************************************************
* 						硬件常量定义
********************************************************************************************************************
*/
#define	Debug				1				// 调试

#define	NOP	  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); \
			  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); \
			  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); \
			  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); \
			  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); \
			  _nop_(); _nop_(); _nop_(); _nop_(); _nop_()

#define		SystemOSCFrq	11059200L	//系统时钟
#define		SystemCLKMode	1			// 1T/12T模式
#define		FOSC			SystemOSCFrq	


#define	SerialSendData(Value)	{ES = 0; REN = 0; TI = 0; SBUF = Value; while (!TI); TI = 0; ES = REN = 1;};


#define 	T_1MS		(1L)
#define 	T_10MS		(10L)
#define 	T_100MS		(100L)
#define 	T_1S		(1000L)
#define 	T_1MM		(T_1S*60L)
	
// ----------------- 工作状态 --------------------------------					
#define MEET_STATE 				0x01 
#define VOTE_STATE 				0x02
#define SIGN_STATE	 			0x03 
#define SET_ID_STATE 			0x04
	
// ----------------- 按键状态 --------------------------------					


// ----------------- LED显示状态 -----------------------------
#define		SYSTEM_LED		0x00	//系统指示灯-双色灯
#define		KEY_LED			0x01	//按键指示灯
#define		COM_LED			0x02	//通信指示灯
#define		LINK_LED		0x03	//连接指示灯

#define		LED_W		0x00	//双色灯：白色
#define		LED_R		0x01	//双色灯：红色
#define		LED_B		0x02	//双色灯：蓝/绿色
#define		LED_Y		0x03	//双色灯：黄色

#define		SYSTEM_LED_ON		1	//输出高电平点亮LED灯
#define		SYSTEM_LED_OFF		0

#define		KEY_LED_ON			1	//输出高电平点亮LED灯
#define		KEY_LED_OFF			0

#define		COM_LED_ON			0	//输出低电平点亮LED灯
#define		COM_LED_OFF			1

#define		LINK_LED_ON			0	//输出低电平点亮LED灯
#define		LINK_LED_OFF		1


// ----------------- 电机 --------------------------------
#define		MT_EN_PWM			1	//是否使用PWM控制电机

#define		MT_CYCLE_MAX		(CYCLE - 1)	//最大周期
#define		MOTOR_CYCLE_1		((MT_CYCLE_MAX/100)*15)	// 15%
#define		MOTOR_CYCLE_2		((MT_CYCLE_MAX/100)*30)	// 30%
#define		MOTOR_CYCLE_3		((MT_CYCLE_MAX/100)*50)	// 50%
#define		MOTOR_CYCLE_4		((MT_CYCLE_MAX/100)*75)	// 75%
#define		MOTOR_CYCLE_5		((MT_CYCLE_MAX/100)*90)	// 90%


// ----------------- 485串口 -------------------------------------
#define SEND_ON 				1 
#define SEND_OFF 				0 

// ----------------- 12V输出供电-控制屏幕背光 ---------------------
#define XSP_12V_ON 				1 
#define XSP_12V_OFF				0 

// ----------------- 限位开关 -------------------------------------
#define SWITCH_ON 				0	//开关闭合
#define SWITCH_OFF 				1	//开关松开


/*
********************************************************************************************************************
* 看门狗控制定时器 WDT_CONTR 		Reset Value xx00 0000
* - WDT_FLAG - Rsvd - EN_WDT - CLR_WDT - IDLE_WDT - PS2 - PS1 - PS0
* -   0  	-  0   -   1    -    1    -     0    -  1   -  1  -  0

//看门狗所溢出时间 = (N * Pre_Scale * 32768) / Freq
//看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
//设置看门狗定时器分频数PS为128,FOSC为11.0592MHz,溢出时间如下:
=(12 * 32768 * 128) / 11059200
=4.5511s
********************************************************************************************************************
*/
#define	FED_DOG_VAL		0x37	// 溢出时间为9.1s


/*
********************************************************************************************************************
* 						宏定义
********************************************************************************************************************
*/
#define	GlobalInterCtl(Val)		EA = Val;

#define	DELAY5US()				_nop_(); _nop_(); _nop_(); _nop_(); _nop_();\
								_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
								
#define	Int1InterEn()		EX1 = 1;
#define	Int1InterDis()		EX1 = 0;
#define Int1TrgMode(Val)	{IT1 = Val;}

/*
********************************************************************************************************************
* 						定时器宏定义
********************************************************************************************************************
*/
#define		START_TIMER0()			TR0 = 1;
#define		STOP_TIMER0()			TR0 = 0;

#define		ENABLE_TIMER0_INTR()	ET0 = 1;
#define		DIABLE_TIMER0_INTR()	ET0 = 0;

#define		START_TIMER1()			TR1 = 1;
#define		STOP_TIMER1()			TR1 = 0;

#define		ENABLE_TIMER1_INTR()	ET1 = 1;
#define		DIABLE_TIMER1_INTR()	ET1 = 0;

/********************************************************************************************************************
**设置定时时间单位us(使用定时器0，工作于模式1下，16位不可重装载模式)
**最大的定时时间为 65536 * SystemCLKMode / (SystemOscFrq / 1000000)
** SystemCLKMode is: 	1 in 1-Clock Mode, 12 in 12-Clock Mode(Defaut)
********************************************************************************************************************/
#define DELAY_1MS_TIME0	1 * 1000
#define T0COUNTER		(65536 - (uint)(((float)DELAY_1MS_TIME0 * SystemOSCFrq) / ((float)SystemCLKMode * 1000000)))

#define DELAY_10MS_TIME0	10 * 1000
#define T0COUNTER2		(65536 - (uint)(((float)DELAY_10MS_TIME0 * SystemOSCFrq) / ((float)SystemCLKMode * 1000000)))

//#define T0_10MS (65536 - (uint)(SystemOSCFrq/100))      //1T模式
#define T0_10MS (65536 - SystemOSCFrq/12/100) //12T模式


/********************************************************************************************************************
**设置定时时间单位us(使用定时器1，工作于模式1下，16位不可重装载模式)
**最大的定时时间为 65536 * SystemCLKMode / (SystemOscFrq / 1000000)
** SystemCLKMode is: 	1 in 1-Clock Mode, 12 in 12-Clock Mode(Defaut)
********************************************************************************************************************/
#define DELAY_1MS_TIME1	1 * 1000
#define T1COUNTER		(65536 - (uint)(((float)DELAY_1MS_TIME1 * SystemOSCFrq) / ((float)SystemCLKMode * 1000000)))

#define DELAY_2000US_TIME1	2000
#define T1COUNTER2		(65536 - (uint)(((float)DELAY_2000US_TIME1 * SystemOSCFrq) / ((float)SystemCLKMode * 1000000)))

#define T1_1MS (65536 - (uint)(SystemOSCFrq/1000))      //1T模式
//#define T1_1MS (65536 - SystemOSCFrq/12/1000) //12T模式


/*
********************************************************************************************************************
* 						PWM常量定义
********************************************************************************************************************
*/
#define	PWM_FRE_100K		0x006FL	//主频11.0592M频率为100k(CNT=111)
#define	PWM_FRE_10K			0x0452L	//主频11.0592M频率为10k(CNT=1106)
#define	PWM_FRE_1K			0x2B34L	//主频11.0592M频率为1k(CNT=11060)

/* 任意周期和任意占空比DUTY%的PWM */
#define CYCLE   PWM_FRE_10K    //定义PWM周期(最大值为65535)
								//16位的PWM计数器，0xFFFF
								//常量后面加L，代表数据类型为长整型，这是为了防止在给反转计数器设置值的时候产生溢出错误。

/*
********************************************************************************************************************
* 						中断向量常量定义
********************************************************************************************************************
*/
#define	EX_INT0_INTR	0	//外部中断0
#define	TIMER0_INTR		1	//定时器0中断
#define	EX_INT1_INTR	2	//外部中断1
#define	TIMER1_INTR		3	//定时器1中断
#define	UART1_INTR		4	//串口1中断
#define	ADC_INTR		5	//ADC中断
#define	LVD_INTR		6	//低电压检测LVD中断
#define	PCA_INTR		7	//PCA中断
#define	UART2_INTR		8	//串口2中断
#define	SPI_INTR		9	//SPI通信中断
#define	EX_INT2_INTR	10	//外部中断2
#define	EX_INT3_INTR	11	//外部中断3
#define	TIMER2_INTR		12	//定时器2中断

#define	EX_INT4_INTR	16	//外部中断4
#define	UART3_INTR		17	//串口3中断
#define	UART4_INTR		18	//串口4中断
#define	TIMER3_INTR		19	//定时器3中断
#define	TIMER4_INTR		20	//定时器4中断
#define	COMP_INTR		21	//比较器中断
#define	PWM_INTR		22	//PWM中断
#define	PWMFD_INTR		23	//PWM异常中断


/*
********************************************************************************************************************
* 						串口中断配置预定义
********************************************************************************************************************
*/
//串口2中断预定义
#define S2RI  0x01
#define S2TI  0x02
#define S2RB8 0x04
#define S2TB8 0x08

//串口3中断预定义
#define S3RI  0x01
#define S3TI  0x02
#define S3RB8 0x04
#define S3TB8 0x08

//串口4中断预定义
#define S4RI  0x01
#define S4TI  0x02
#define S4RB8 0x04
#define S4TB8 0x08

/*
********************************************************************************************************************
* 						24C02数据存放地址定义
********************************************************************************************************************
*/

#define SET_ID_ADR				0xA0 			// 是否设置ID
#define MAC_ID_ADDRH			0xA1			// ID高位
#define MAC_ID_ADDRL 			0xA2			// ID低位

#define	DEBUG_ADDR				0xF0			// 测试起始地址

/*
********************************************************************************************************************
* 						FLASH 数据存放地址定义
********************************************************************************************************************
*/


#endif
