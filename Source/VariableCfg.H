/*
********************************************************************************************************************
*文件名:   	VariableCfg.H
*应用语言: 	KEIL C51
*应用系统:	无系统
*版本 :   	V1.0
*作者：    	
********************************************************************************************************************
*/
#ifndef _VARIABLECFG_H_
#define	_VARIABLECFG_H_

#include "Typedef.h"

/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _VARIABLECFG_GLOBAL_
	#define	VARIABLECFG_GLOBAL	extern
#else
	#define	VARIABLECFG_GLOBAL
#endif

/*
*********************************************************************************
* Serial 应用定义
*********************************************************************************
*/
typedef     struct     	_serialframestruct_{
	uchar	Var1;			// 参数1
	uchar	Var2;			// 参数2
	uchar	Var3;			// 参数3
	uchar	DesAdr;			// 目标地址，分组编号
	uchar	Tail;			// 结束符
} SerialFrameStruct, *P_SerialFrameStruct;

//应用结构
VARIABLECFG_GLOBAL	SerialFrameStruct	data   ComRcvFrame;			// HardSerial Rcv Frame
VARIABLECFG_GLOBAL	SerialFrameStruct	data   ComTransFrame;		// HardSerial Trans Frame

// 接收缓冲区定义
#define		SERIAL_FRAME_SIZE		10			// 最大10帧
VARIABLECFG_GLOBAL	uchar 		data   SerialWritePoint;			// 接收缓冲区读指针
VARIABLECFG_GLOBAL	uchar 		data   SerialReadPoint;				// 接收缓冲区写指针
VARIABLECFG_GLOBAL	SerialFrameStruct  xdata   ComRcvFrameBuf[SERIAL_FRAME_SIZE];

VARIABLECFG_GLOBAL	uchar		data	SerialRcvBuff[8];			// 串口临时接收缓冲区
VARIABLECFG_GLOBAL	uchar		data	SerialSndBuff[4];			// 串口发送缓冲区
VARIABLECFG_GLOBAL	uchar		data	SerialRcvCnt;
VARIABLECFG_GLOBAL	uchar		data	SerialSndCnt;
VARIABLECFG_GLOBAL  bit			SerialRcvOkFlag;      				// 串口正确接收标志
	
// 从机的状态信息
typedef		struct _slaverinforstruct_ {
    uint  MacId;			// 本机Id，被分组的ID
    uchar SysMode;			// 系统模式		0: ID设置，1：会议，2：签到，3：表决, 4:空闲	
   	uchar MacSta;			// 机器状态				0: 表示被关闭，		 1：正常工作
} SlaverInforStruct, *P_SlaverInforStruct;

VARIABLECFG_GLOBAL	SlaverInforStruct	SlaverInforFrame;


/*
***********************************************************************************
*定时器应用变量定义
***********************************************************************************
*/
VARIABLECFG_GLOBAL uchar idata	Timer0IntrSum;			// Timer0 Interrupt Times
VARIABLECFG_GLOBAL uchar idata	Timer0InterFlag;		// Timer0 Interrupt Flag

/*
***********************************************************************************
*						LED应用变量定义
***********************************************************************************
*/
VARIABLECFG_GLOBAL uchar data	LedStartBlink;			// LED开始闪烁的标志
VARIABLECFG_GLOBAL uchar data	LedBlinkCnt;			// LED闪烁周期计数器

/*
********************************************************************************************************************
* 						Watch Dog 应用变量定义
********************************************************************************************************************
*/
VARIABLECFG_GLOBAL bit		WdtClrEnaFlag;
VARIABLECFG_GLOBAL uchar	idata	WdtClrCount;

/*
********************************************************************************************************************
* 						Key应用变量定义
********************************************************************************************************************
*/
VARIABLECFG_GLOBAL	uint 	data 	KeyValue;
VARIABLECFG_GLOBAL	uchar	data	KeyIndex;
VARIABLECFG_GLOBAL	uchar	data	sKeyIndex;
VARIABLECFG_GLOBAL	bit		KeyTypeFlag;			// 按键类型，0x00：短按，0xCC：长按
VARIABLECFG_GLOBAL	bit		KeyOprOkFlag;			
VARIABLECFG_GLOBAL	bit		LongKeyPush;			// 长按键按下标志
VARIABLECFG_GLOBAL	bit		KeyPushPopFlg;			// 按键按下/弹起标志 1：按下，0：弹起

VARIABLECFG_GLOBAL	uint 	data 	KeyDelay10Ms;
VARIABLECFG_GLOBAL	uchar	data	KeyLockStatus;	// 按键锁定状态
VARIABLECFG_GLOBAL	uint 	data 	KeyKeepMs;

/*
********************************************************************************************************************
* 						电机应用变量定义
********************************************************************************************************************
*/
typedef enum	//电机选项
{
	MT = 0,	//	默认电机
	MA = 1,
	MB = 2,
	MC = 3,
	MD = 4,
}eMotorEn;

typedef enum	//电机输出通道选项
{
	M12 = 0,
	M34 = 1,
}eMotorOut;

typedef enum	//电机转动方向选项
{
	CW  = 0, //顺时针方向
	CCW = 1, //逆时针方向
	STOP = 2,	//停止
	BREAK = 3,	//刹车
}eMotorDir;

typedef enum	//电机运行状态选项
{
	FREE = 0,
	RUN  = 1,
}eMotorMode;

typedef	struct {
	eMotorEn		EnDes;			// 电机使能目标
	eMotorOut		Out;			// 电机输出通道
	eMotorDir		Dir;			// 电机转动方向
	eMotorMode		Mode;			// 电机运行状态
	uint			Cycle;			// 电机运行周期
} structMotorUnitInfo;

//应用结构
VARIABLECFG_GLOBAL	structMotorUnitInfo	data   sMotorFirst;			// 端接电机1
VARIABLECFG_GLOBAL	structMotorUnitInfo	data   sMotorSecond;		// 端接电机2


VARIABLECFG_GLOBAL 	uint 	data 	mtRunTimCnt;//电机运行时间计数
VARIABLECFG_GLOBAL 	uint 	data 	mtRunCycle;//电机运行的周期
VARIABLECFG_GLOBAL 	uchar 	data 	mtRunDir;	//电机运行的方向

/*
***********************************************************************************
*应用变量定义
***********************************************************************************
*/
/************************** 编辑ID功能的变量 **************************************/


/************************** UART冲突检测/重发功能的变量 **************************************/




/************************** 系统的变量 **************************************/
VARIABLECFG_GLOBAL volatile	ulong  	uwTick;	//电机运行的方向

VARIABLECFG_GLOBAL  uint 	data  	time10msCnt;	//10ms计数


#endif
	