#define	_IICAPP_GLOBAL_

#include <stdio.h>  

#include "Public.h"
#include "AT24C01_02.H"
#include "HardWare.H"


/*--------------------------------------------------------------------------------------------------*/
/*-------------------------------------���¶��ǲ��Գ���---------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/

static void ee_Delay(uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

//**************************************************
//��������: ������ƬEERPOM
//����ֵ:	��
//**************************************************
void ee_Erase(void)
{
	u16 i;
	u8 buf[EE_SIZE];
	
	/* ��仺���� */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	
	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	if (WrToRomA(SlaveADDR, 0, buf, EE_SIZE) != 0)	
	{
		printf("����eeprom����!\r\n");
		return;
	}
	else
	{
		printf("����eeprom�ɹ�!\r\n");
	}
}

//**************************************************
//��������: AT24C02 ��д����20���ַ�
//����ֵ:	��
//**************************************************
void AT24C02_Test(void)
{
	u8 i;
	static u8 test_data[20] = {
		0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0,
		0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xA1,
	};

	//������ַ���洢��Ԫ��ַ������ָ�룬д���ֽ���
	WrToRomA(SlaveADDR, 0x05, test_data, 20);
	for(i = 0; i < 20; i++)
	{
		test_data[i] = 0x00;
//		u1SendByte(test_data[i]);
		printf("%02bX ", test_data[i]);
	}
	
	//������ȡ20���ֽ�����
	RdFromROM(SlaveADDR, 0x05, test_data, 20);
	for(i = 0; i < 20; i++)
	{
//		u1SendByte(test_data[i]);
		printf("%02bX ", test_data[i]);
	}	
}

//**************************************************
//��������: AT24C02 ��д����5���ַ���ָ���ĵ�ַ
//����ֵ:	��
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
//��������: AT24C02 ���Ժ�����ָ���ĵ�ַ
//����ֵ:	��
//**************************************************
void AT24C02DebugPrg(void)
{
	uchar i;
	
	for (i = 0; i < 5; i++) {
		write_I2C(0xCC + i, DEBUG_ADDR + i); 
	}
	for (i = 0; i < 5; i++) {
		//u1SendByte(read_I2C(DEBUG_ADDR + i));
		printf("%02bX ",read_I2C(DEBUG_ADDR + i));//%x/%X,�������unsigned int����C51�д�ӡһ��unsigned char ��Ҫʹ��bx/bX
	}
}

#if 1
/*
 * eeprom AT24C02 ��д����
 */
void ee_Test(void)
{
	uint16_t i;
	static uint8_t write_buf[EE_SIZE] = {0};
	static uint8_t read_buf[EE_SIZE] = {0};

	/*-----------------------------------------------------------------------------------*/  
	if (ee_CheckOk() != 0)
	{
		/* û�м�⵽EEPROM */
		printf("û�м�⵽����EEPROM!\r\n");
				
		return;	/* ���� */
	}
	/*------------------------------------------------------------------------------------*/  
	/* �����Ի����� */
	for (i = 0; i < EE_SIZE; i++)
	{		
		write_buf[i] = i;
		WrToRomA(SlaveADDR, i, &write_buf[i], 1);
	}
	/*------------------------------------------------------------------------------------*/  
//	if (WrToRomA(SlaveADDR, 0, write_buf, EE_SIZE) != 0)
//	{
//		printf("дeeprom����!\r\n");
//		return;
//	}
//	else
//	{
		printf("дeeprom�ɹ�!\r\n");
//	}

	/*д��֮����Ҫ�ʵ�����ʱ��ȥ������Ȼ�����*/
	ee_Delay(0x0FFFFF);
	/*-----------------------------------------------------------------------------------*/
	if (RdFromROM(SlaveADDR, 0, read_buf, EE_SIZE) != 0)
	{
		printf("��eeprom����!\r\n");
		return;
	}
	else
	{
		printf("��eeprom�ɹ�����������:\r\n");
	}
	/*-----------------------------------------------------------------------------------*/  
	for (i = 0; i < EE_SIZE; i++)
	{
		if(read_buf[i] != write_buf[i])
		{
			printf("0x%02bX \r\n", read_buf[i]);
			printf("����: EEPROM������д������ݲ�һ��\r\n");
			return;
		}
		printf(" %02bX", read_buf[i]);
		
		if ((i & 15) == 15)
		{
			printf("\r\n");	
		}
	}
	printf("eeprom��д���Գɹ�!\r\n");
}
#endif

#endif
