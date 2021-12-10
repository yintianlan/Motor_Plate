#ifndef _GPIO_DEFINE_H_
#define	_GPIO_DEFINE_H_

#include <STC8H.h>


/*
********************************************************************************************************************
* 						外设IO 绑定
********************************************************************************************************************
*/
// Key IO 定义
//#define	KeyReadPort			P0

// I2C IO 定义
#define SDA 		P24	//I2C SDA
#define SCL 		P25	//I2C CLK

#define IO_LED_R	P26 //双色LED RED
#define IO_LED_B	P27 //双色LED BLUE

#define IO_JC_AD	P00	//检测电机驱动的AD
#define IO_KEY_AD 	P01	//按键AD

#define IO_KEY_LED 	P02	//按键LED IO
#define IO_PM_LED1 	P03	//串口LED IO
#define IO_PM_LED0 	P10	//串口LED IO
#define IO_VER_AD 	P11	//AD采集口，用于读取版本

#define IO_RXD		P30	//log串口RX
#define IO_TXD		P31	//log串口TX

#define IO_HW1 		P55	//红外对射
#define IO_HW2 		P32	//红外对射
#define IO_XSP 		P33	//控制12V电压...

#define IO_RS485_EN	P35	//485方向控制
#define IO_RS485_RX	P36	//485串口接收
#define IO_RS485_TX	P37	//485串口发送

// 电机及其驱动使能 IO 定义
#define IO_MT1 		P23	//电机输出1
#define IO_MT2 		P22	//电机输出2
#define IO_MT3 		P21	//电机输出3
#define IO_MT4 		P20	//电机输出4


#endif
