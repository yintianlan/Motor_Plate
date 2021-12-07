#define	_UART_GLOBAL_

#include "UartFunc.H"
#include "HardWare.H"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uchar xdata u1RecBuf[256]={0};//串口接收BUFF
uchar xdata u2RecBuf[256]={0};

uint rec_cnt=0;//串口接收计数
uint u2rec_cnt=0;

volatile uchar u1busy=0;//串口忙标志位
volatile uchar u2busy=0;


/* 实现串口发送 */
void u1SendByte(char dat){u1busy=1;SBUF = dat; while (u1busy);}
void u2SendByte(char dat){u2busy=1;S2BUF = dat;while (u2busy);}

void u1Send(char *dat,u16 len){while(len--)u1SendByte(*dat++);}
void u2Send(char *dat,u16 len){while(len--)u2SendByte(*dat++);}

void u1SendStr(char *dat){u1Send(dat,strlen(dat));}
void u2SendStr(char *dat){u2Send(dat,strlen(dat));}


//实现打印函数
char putchar(char ch)
{
	u1SendByte(ch);
//	SerialSendData(ch);
	return ch;
}


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

// 使用定时器2做串口1的波特率发生器
void uart1_init(ulong BAUD)	 //115200bps@11.0592MHz
{
	SCON = 0x50;                //8位可变波特率,SCON: mode 1, 8-bit UART, enable Reciver
    AUXR |= 0x04;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器	
    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x10; 	//启动定时器2
	
    ES = 1;                     //使能串口1中断
}
	

/* 串口2 初始化 */
void u2Init(void)	 //115200bps@11.0592MHz
{
	S2CON = 0x50;
	AUXR |= 0x04;
	T2L = 0xE8;
	T2H = 0xFF;
	AUXR |= 0x10;

	IE2 |= 0x01;                     //使能串口2中断
}



#if 0
/********串口1 中断服务程序************/
void U1IRQ(void)interrupt UART1_INTR
{
	static char cnt7f=0;

	if (TI)//发送中断
	{
		u1busy=0;
		TI=0;
	}
	
	if(RI)//接收中断
	{
		char dat=SBUF;
		
		u1RecBuf[rec_cnt++%sizeof(u1RecBuf)]=dat;
		if(dat==0x7f)
		{
			if(++cnt7f>=16)IAP_CONTR = 0x60;
		}
		else
		{
			cnt7f=0;
		}
		
		RI=0;
	}
}
#endif

/********串口2 中断服务程序************/
void U2IRQ(void)interrupt UART2_INTR
{
	if (S2CON & S2RI)
	{
		char dat = S2BUF;
		u2RecBuf[u2rec_cnt++%sizeof(u2RecBuf)]=dat;
		S2CON &= ~S2RI;
		
	}
	if (S2CON & S2TI)
	{
		u2busy=0;
		S2CON &= ~S2TI;
	}
}

#endif
