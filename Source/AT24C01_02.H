#ifndef __AT24C01_02_H_
#define __AT24C01_02_H_

#include "Typedef.h"

/*
********************************************************************************************************************
* 						变量及函数的引用定义
********************************************************************************************************************
*/
#ifndef _IICAPP_GLOBAL_
	#define	IICAPP_GLOBAL	extern
#else
	#define	IICAPP_GLOBAL
#endif

	
/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

//从机地址，格式：1010 ***0
#define SlaveADDR	0xA0
	
/* AT24C01/02每页有8个字节 
 * AT24C04/08A/16A每页有16个字节 
 */
#define EE_DEV_ADDR			SlaveADDR	/* 24xx02的设备地址 */
#define EE_PAGE_SIZE		8			/* 24xx02的页面大小 */
#define EE_SIZE				256			/* 24xx02总容量 */


#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */


bit WrToRomA(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len);	//慢写
bit WrToRomB(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len);	//快写
bit RdFromROM(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len);	//快读

//AT24C02 检测芯片函数
u8 i2c_CheckDevice(u8 SlaveAddress);
u8 ee_CheckOk(void);

//AT24C02 测试函数
void ee_Erase(void);
void ee_Test(void);
void AT24C02_Test(void);
void AT24C02_DebugPrg(void);
void AT24C02DebugPrg(void);


/************************************************************************************
*函数介绍：写1个字节的数据到指定的地址
*输入参数：待写的数据、指定的地址
*输出参数：无
*返回值  : 无
*************************************************************************************/
IICAPP_GLOBAL void write_I2C(uchar T_Data, uchar RomAddress);

/************************************************************************************
*函数介绍：从指定的地址读1个字节的数据
*输入参数：指定的地址
*输出参数：无
*返回值  : 无
*************************************************************************************/
IICAPP_GLOBAL uchar read_I2C(uchar RomAddress);


#endif
