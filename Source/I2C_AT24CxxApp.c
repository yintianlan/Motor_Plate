#define	_IICAPP_GLOBAL_

#include <stdio.h>  

#include "Public.h"
#include "AT24C01_02.H"
#include "HardWare.H"


/*--------------------------------------------------------------------------------------------------*/
/*-------------------------------------以下都是测试程序---------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/

static void ee_Delay(uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

//**************************************************
//功能描述: 擦除整片EERPOM
//返回值:	无
//**************************************************
void ee_Erase(void)
{
	u16 i;
	u8 buf[EE_SIZE];
	
	/* 填充缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	
	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	if (WrToRomA(SlaveADDR, 0, buf, EE_SIZE) != 0)	
	{
		printf("擦除eeprom出错!\r\n");
		return;
	}
	else
	{
		printf("擦除eeprom成功!\r\n");
	}
}

//**************************************************
//功能描述: AT24C02 读写测试20个字符
//返回值:	无
//**************************************************
void AT24C02_Test(void)
{
	u8 i;
	static u8 test_data[20] = {
		0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0,
		0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xA1,
	};

	//器件地址，存储单元地址，数据指针，写入字节数
	WrToRomA(SlaveADDR, 0x05, test_data, 20);
	for(i = 0; i < 20; i++)
	{
		test_data[i] = 0x00;
//		u1SendByte(test_data[i]);
		printf("%02bX ", test_data[i]);
	}
	
	//连续读取20个字节数据
	RdFromROM(SlaveADDR, 0x05, test_data, 20);
	for(i = 0; i < 20; i++)
	{
//		u1SendByte(test_data[i]);
		printf("%02bX ", test_data[i]);
	}	
}

//**************************************************
//功能描述: AT24C02 读写测试5个字符，指定的地址
//返回值:	无
//**************************************************
void AT24C02_DebugPrg(void)
{
	uchar i,icdata = 0;
	
	for (i = 0; i < 5; i++) {
		icdata = 0xCC + i;
		WrToRomA(SlaveADDR, DEBUG_ADDR + i, &icdata, 1);
	}
	for (i = 0; i < 5; i++) {
		RdFromROM(SlaveADDR, DEBUG_ADDR + i, &icdata, 1);
//		u1SendByte(icdata);
		printf("0x%02X ", icdata);
	}
}

//**************************************************
//功能描述: AT24C02 测试函数，指定的地址
//返回值:	无
//**************************************************
void AT24C02DebugPrg(void)
{
	uchar i;
	
	for (i = 0; i < 5; i++) {
		write_I2C(0xCC + i, DEBUG_ADDR + i); 
	}
	for (i = 0; i < 5; i++) {
		//u1SendByte(read_I2C(DEBUG_ADDR + i));
		printf("%02bX ",read_I2C(DEBUG_ADDR + i));//%x/%X,输出的是unsigned int，在C51中打印一个unsigned char 需要使用bx/bX
	}
}

#if 1
/*
 * eeprom AT24C02 读写测试
 */
void ee_Test(void)
{
	uint16_t i;
	static uint8_t write_buf[EE_SIZE] = {0};
	static uint8_t read_buf[EE_SIZE] = {0};

	/*-----------------------------------------------------------------------------------*/  
	if (ee_CheckOk() != 0)
	{
		/* 没有检测到EEPROM */
		printf("没有检测到串行EEPROM!\r\n");
				
		return;	/* 返回 */
	}
	/*------------------------------------------------------------------------------------*/  
	/* 填充测试缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{		
		write_buf[i] = i;
		WrToRomA(SlaveADDR, i, &write_buf[i], 1);
	}
	/*------------------------------------------------------------------------------------*/  
//	if (WrToRomA(SlaveADDR, 0, write_buf, EE_SIZE) != 0)
//	{
//		printf("写eeprom出错!\r\n");
//		return;
//	}
//	else
//	{
		printf("写eeprom成功!\r\n");
//	}

	/*写完之后需要适当的延时再去读，不然会出错*/
	ee_Delay(0x0FFFFF);
	/*-----------------------------------------------------------------------------------*/
	if (RdFromROM(SlaveADDR, 0, read_buf, EE_SIZE) != 0)
	{
		printf("读eeprom出错!\r\n");
		return;
	}
	else
	{
		printf("读eeprom成功，数据如下:\r\n");
	}
	/*-----------------------------------------------------------------------------------*/  
	for (i = 0; i < EE_SIZE; i++)
	{
		if(read_buf[i] != write_buf[i])
		{
			printf("0x%02bX \r\n", read_buf[i]);
			printf("错误: EEPROM读出与写入的数据不一致\r\n");
			return;
		}
		printf(" %02bX", read_buf[i]);
		
		if ((i & 15) == 15)
		{
			printf("\r\n");	
		}
	}
	printf("eeprom读写测试成功!\r\n");
}
#endif

#endif
