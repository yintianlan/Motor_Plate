#define	_PUBLIC_GLOBAL_

/*
********************************************************************************************************************
* 						导入头文件
********************************************************************************************************************
*/
#include "Public.h"

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "UartFunc.H"



//实现打印函数
char putchar(char ch)
{
	u1SendByte(ch);
//	SerialSendData(ch);
	return ch;
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void HAL_GPIO_Init(void)
{
	/* 端口模式
	 * M1M0:00--准双向口(传统8051 I/O口) 
			01--推挽输出 
			10--高阻输入 
			11--开漏(可读可输出，但需要外加上拉电阻)
	*/
	P0M0=0x0C; //0000 1100
	P0M1=0x03; //0000 0011

	P1M0=0x01; //0000 0001
	P1M1=0x02; //0000 0010

	P2M0=0xF0; //1111 0000
	P2M1=0x30; //0011 0000

	P3M0=0x28; //0010 1000
	P3M1=0x45; //0100 0101

	P4M0=0x00; //0000 0000
	P4M1=0x00; //0000 0000

	P5M0=0x00; //0000 0000
	P5M1=0x20; //0010 0000
}

/**
  * @brief Provides a tick value in millisecond.
  * @retval tick value
  */
ulong HAL_GetTick(void)
{
  return uwTick;
}

/*****************************************************************************
**Name:		 	ReadUserTimer
**Function:	 	获取当前的滴答计数
**Args:		A pointer to get the current system tick
**Return:	None
******************************************************************************/
void ResetUserTimer(ulong *Timer)
{
	*Timer = HAL_GetTick();
}

/*****************************************************************************
**Name:		 	ReadUserTimer
**Function:	 	读取当前系统的时间，返回（当前系统滴答计时-用户记录的时间数），
**Args:
**Return:		
******************************************************************************/
ulong ReadUserTimer(ulong *Timer)
{
	ulong tmp = HAL_GetTick();
	return (*Timer <= tmp) ? (tmp - *Timer) : (0xFFFFFFFF - tmp + *Timer);
}


#endif
