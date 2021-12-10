#define	_UART_GLOBAL_

#include "UartFunc.H"
#include "HardWare.H"
#include "GpioDefine.H"

#include <string.h>


uchar xdata u1RecBuf[256]={0};//串口接收BUFF
uchar xdata u2RecBuf[256]={0};

uint rec_cnt=0;//串口接收计数
uint u2rec_cnt=0;

volatile uchar u1busy=0;//串口忙标志位
volatile uchar u2busy=0;


/* 实现串口发送 */
void u1SendByte(char dat){IO_RS485_EN = SEND_ON;u1busy=1;SBUF = dat; while (u1busy);IO_RS485_EN = SEND_OFF;}
//void u1SendByte(char dat){u1busy=1;SBUF = dat; while (u1busy);}
void u2SendByte(char dat){u2busy=1;S2BUF = dat;while (u2busy);}

void u1Send(char *dat,u16 len){while(len--)u1SendByte(*dat++);}
void u2Send(char *dat,u16 len){while(len--)u2SendByte(*dat++);}

void u1SendStr(char *dat){u1Send(dat,strlen(dat));}
void u2SendStr(char *dat){u2Send(dat,strlen(dat));}


/* 串口1 初始化 */
/*	SCON	B7		B6		B5		B4		B3		B2		B1		B0
 *		  SM0/FE	SM1		SM2		REN		TB8		RB8		TI		RI
 *			0		1		0		1		0		0		0		0
 * 使用定时器1做串口1的波特率发生器
 */
void u1Init(void)	 //115200bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE8;		//设置定时初始值
	TH1 = 0xFF;		//设置定时初始值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时

	ES=1;
}

//// 使用定时器2做串口1的波特率发生器
//void uart1_init(ulong BAUD)	//115200bps@11.0592MHz
//{
//	SCON = 0x50;				//8位可变波特率,SCON: mode 1, 8-bit UART, enable Reciver
//	AUXR |= 0x04; 				//T2为1T模式, 并启动定时器2
//	AUXR |= 0x01; 				//选择定时器2为串口1的波特率发生器	
//	T2L = (65536 - (FOSC/4/BAUD));	//设置波特率重装值
//	T2H = (65536 - (FOSC/4/BAUD))>>8;
//	AUXR |= 0x10; 	//启动定时器2
//	
//	ES = 1;							//使能串口1中断
//}

// 使用定时器2做串口1的波特率发生器
void uart1_init(ulong BAUD)	 // @11.0592MHz
{
	/*********** 波特率使用定时器2 *****************/
	AUXR |= 0x01;		//S1 BRT Use Timer2;
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	T2L = (65536 - (FOSC/4/BAUD));
	T2H = (65536 - (FOSC/4/BAUD))>>8;
	IE2  &= ~(1<<2);	//禁止中断
	AUXR |=  (1<<4);	//Timer run enable
	/*************************************************/

	SCON = (SCON & 0x3f) | 0x40;	//UART1模式,0x00: 同步移位输出, 
									//			0x40: 8位数据,可变波特率, 
									//			0x80: 9位数据,固定波特率,
									//			0xc0: 9位数据,可变波特率
//	PS	= 1;	//高优先级中断
	ES	= 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 &= 0x3f;
//	P_SW1 |= 0x00;	
	P_SW1 |= 0x40;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (必须使用内部时钟)
//	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
}

/* 串口2 初始化 */
void u2Init(void)	 //115200bps@11.0592MHz
{
	S2CON = 0x50;
	AUXR |= 0x04;
	T2L = 0xE8;
	T2H = 0xFF;
	AUXR |= 0x10;

	IE2 |= 0x01;					//使能串口2中断
}

// 使用定时器2做串口2的波特率发生器
void uart2_init(ulong BAUD)	 //115200bps@11.0592MHz
{
	SCON = 0x50;				//8位可变波特率,SCON: mode 1, 8-bit UART, enable Reciver
	AUXR |= 0x04;				//T2为1T模式, 并启动定时器2
	AUXR |= 0x01;				//选择定时器2为串口2的波特率发生器	
	T2L = (65536 - (FOSC/4/BAUD));	//设置波特率重装值
	T2H = (65536 - (FOSC/4/BAUD))>>8;
	AUXR |= 0x10;	//启动定时器2
	
	IE2 |= 0x01;					//使能串口2中断
}

#if 0
/********串口1 中断服务程序************/
void U1IRQ(void)interrupt UART1_INTR
{
	static char cnt7f = 0;

	if (TI)//发送中断
	{
		u1busy = 0;
		TI = 0;
	}
	
	if(RI)//接收中断
	{
		char dat = SBUF;
		
		u1RecBuf[rec_cnt++%sizeof(u1RecBuf)] = dat;
		if(dat == 0x7f)
		{
			if(++cnt7f>=16)IAP_CONTR = 0x60;
		}
		else
		{
			cnt7f = 0;
		}
		
		RI = 0;
	}
}
#endif

/********串口2 中断服务程序************/
void U2IRQ(void)interrupt UART2_INTR
{
	if (S2CON & S2RI)
	{
		char dat = S2BUF;
		u2RecBuf[u2rec_cnt++%sizeof(u2RecBuf)] = dat;
		S2CON &= ~S2RI;	//Clear Rx flag
	}
	if (S2CON & S2TI)
	{
		u2busy = 0;
		S2CON &= ~S2TI;	//Clear Tx flag
	}
}

#endif
