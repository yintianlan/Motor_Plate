#define	_SERIALAPP_GLOBAL_

#include "HardWare.H"
#include "VariableCfg.H"
#include "SerialApp.H"

#include "KeyApp.H"
#include "Delay.H"
#include "TimeBase.H"
#include "UartFunc.H"
#include "MotorDirver.H"

#include <stdio.h>


/*
************************************************************************************************************************
**函数原型:  	void SerialVarInit(void)            
**参数说明:  	无
**返回值:    	无
**说明:			该函数用于初始化串口部分的变量              				   
************************************************************************************************************************
*/
void SerialVarInit(void)
{
}

/*
************************************************************************************************************************
**函数原型:  	void InitSerialBaudRate(ulong BaudRateValue)            
**参数说明:  	无
**返回值:    	无
**说明:			该函数用于设置UART的波特率              				   
************************************************************************************************************************
*/
void InitSerialBaudRate(ulong BaudRateValue)		
{
	uart1_init((ulong)BaudRateValue);
}

/************************************************************************************
*函数介绍：串口中断程序
*输入参数：无
*输出参数：无
*返回值  ：无
*************************************************************************************/
#if 1
void SerialInterEvent(void)	interrupt	UART1_INTR 
{
	uchar data buff;
//	uint  tAddr;

//	GlobalInterCtl(0);			// Disable Global Interrupt

	if(RI) {					// 接收中断
		RI = 0;					// 清除RI位
		buff = SBUF;			// 接收数据
		
		SerialRcvBuff[SerialRcvCnt] = buff;
		SerialRcvCnt++;
				
		if((SerialRcvCnt >= 0x05) && (SerialRcvBuff[SerialRcvCnt - 5] == 0xFF)) {		// 数据接收正确
			if((SerialRcvBuff[SerialRcvCnt - 4] == 0x10) && (SerialRcvBuff[SerialRcvCnt - 3] == 0x11)) {//协议格式正确
				if(SerialWritePoint >= SERIAL_FRAME_SIZE)	// 循环链路缓冲
					SerialWritePoint = 0;
				ComRcvFrameBuf[SerialWritePoint].Var1	= SerialRcvBuff[SerialRcvCnt - 5];
				ComRcvFrameBuf[SerialWritePoint].Var2	= SerialRcvBuff[SerialRcvCnt - 4];
				ComRcvFrameBuf[SerialWritePoint].Var3	= SerialRcvBuff[SerialRcvCnt - 3];			
				ComRcvFrameBuf[SerialWritePoint].DesAdr	= SerialRcvBuff[SerialRcvCnt - 2];			
				ComRcvFrameBuf[SerialWritePoint].Tail	= SerialRcvBuff[SerialRcvCnt - 1];
				
				SerialWritePoint++;
				if(SerialWritePoint >= SERIAL_FRAME_SIZE)
					SerialWritePoint = 0;
				SerialRcvOkFlag = TRUE;		//接收OK				
			}
			SerialRcvCnt = 0x00;//接收计数清零			
		}
		else {
			if (SerialRcvCnt > 7) SerialRcvCnt = 0x00;
		}
	}
	
	if (_testbit_(TI)) {		// 发送中断
		TI = 0;					// 关闭串口发送中断，清除TI位
		u1busy = 0;				// 清除忙标志位
	}

//	GlobalInterCtl(1);			// Enable Global Interrupt
}
#endif

/*
************************************************************************************************************************
**函数原型:  	SendBuffInit(uint Addr, uchar Cmd)
**参数说明:  	无
**返回值:    	无
**说明:			该函数用于初始化串口发送缓冲区             				   
************************************************************************************************************************
*/
void SendBuffInit(uint Addr, uchar Cmd)
{
	GlobalInterCtl(0);				// Disable Global Interrupt
	SerialSndBuff[0] = 0xFF;
	SerialSndBuff[1] = 0x10;
	SerialSndBuff[2] = 0x11;
	SerialSndBuff[3] = Addr & 0xFF;
	SerialSndBuff[4] = Cmd;
	GlobalInterCtl(1);				// Disable Global Interrupt
}

/************************************************************************************
*函数介绍：void MotorCmdPrg(uchar Cmmd)
*输入参数：Cmmd:命令
*输出参数：无
*返回值    处理各种操作电机的命令
*************************************************************************************/
void MotorCmdPrg(uchar Cmmd)
{
	printf("Motor Cmd: 0x%02bX\r\n", Cmmd);
	switch (Cmmd) {
		case 0xCC:			// 统一暂停
				LifterDirSet(M_STOP);
			break;
		
		case 0xDD:			// 统一上升
				LifterDirSet(M_RISING);
			break;

		case 0xEE:			// 统一下降
				LifterDirSet(M_DECLINING);
			break;
		
		default:
			break;
	}
}

/************************************************************************************
*函数介绍：void SetGroupCmdPrg(uchar Adr)
*输入参数：Adr : 地址
*输出参数：无
*返回值    设置分组命令
*************************************************************************************/
void SetGroupCmdPrg(uchar Adr)
{
	printf("Set Group: 0x%02bX\r\n", Adr);
	if((Adr > 0) && (Adr != SlaverInforFrame.MacId)) {
		SlaverInforFrame.SysMode = SET_ID_STATE;		// ID模式
//		SetIdValInit();									// 设置ID模式下相关变量初始化
//		MacMotorCloseHandle();							// 强制关闭升降器的电机
//		MacSetAplyParaHandle(FALSE);					// 清除机器申请相关变量
//		LedDisplayPrg(SET_ID_STATE, LED_SET_ID);		// 显示设置ID相关的LED状态
	
	}
		
}

/************************************************************************************
*函数介绍：处理485网络接收到的指令
*输入参数：接收帧
*输出参数：无
*返回值  ：无
*************************************************************************************/
void SerialCmdPrg(SerialFrameStruct *ComFrame)
{
	uint Cmd, Addr;

	printf("Rev: %02bX %02bX %02bX %02bX %02bX\r\n", \
		ComFrame->Var1, ComFrame->Var2, ComFrame->Var3, ComFrame->DesAdr, ComFrame->Tail);

	Addr = ComFrame->DesAdr;
	Cmd = ComFrame->Tail;
	
	//机器被锁定，且接收的不是解锁命令
	if ((SlaverInforFrame.MacSta == FALSE) && (Cmd != 0xFCFC)) {
		return;
	}
	
	//匹配协议头
	if((ComFrame->Var1 == 0xFF) && (ComFrame->Var2 == 0x10) && (ComFrame->Var3 == 0x11)) {
		if(ComFrame->DesAdr == 0x00) {
			//统一控制所有升降器电机命令
			MotorCmdPrg(Cmd);
		} 
		else {
			if(Cmd == 0xAA) {
				//设置分组命令
				SetGroupCmdPrg(Addr);
			}
		} 
	}
}	

/*
************************************************************************************************************************
**函数原型:  	void SerialFunctionPrg(void)                    
**参数说明:  	无
**返回值:    	无
**说明:			Process Device Information Program      				   
************************************************************************************************************************
*/ 
void SerialFunctionPrg(void)
{
	if (SerialWritePoint != SerialReadPoint) {
		if ((SerialWritePoint >= SERIAL_FRAME_SIZE) || (SerialReadPoint > SERIAL_FRAME_SIZE)) {
			SerialWritePoint = 0x00;
			SerialReadPoint	 = 0x00;
		}
		else {
			while (1) {
				SerialCmdPrg(&ComRcvFrameBuf[SerialReadPoint]);
				
				SerialReadPoint++;
				if (SerialReadPoint >= SERIAL_FRAME_SIZE)
					SerialReadPoint = 0x00;
				if (SerialReadPoint == SerialWritePoint) 
					break;
			}
		}
	}
}


