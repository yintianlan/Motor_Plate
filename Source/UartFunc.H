#ifndef _UART_FUNC_H_
#define	_UART_FUNC_H_

#include "Typedef.h"


/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _UART_GLOBAL_
	#define	UART_GLOBAL	extern
#else
	#define	UART_GLOBAL
#endif
	
UART_GLOBAL	uchar xdata u1RecBuf[256];
UART_GLOBAL	uint rec_cnt;
UART_GLOBAL	volatile uchar u1busy;

void uart1_init(ulong BAUD);
	
void u1Init(void);	//115200bps@11.0592MHz
void u2Init(void);	//115200bps@11.0592MHz

void u1SendByte(char dat);
void u2SendByte(char dat);
void u1Send(char *dat,u16 len);
void u2Send(char *dat,u16 len);
void u1SendStr(char *dat);
void u2SendStr(char *dat);

	
#endif
