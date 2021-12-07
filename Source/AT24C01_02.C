#include "AT24C01_02.H"

#include "IIC.H"
#include "GpioDefine.H"
#include "Delay.H"


//**************************************************
//功能描述: 内部函数，页写，不能跨页，也可只写单个字节
//参数说明: SlaveAddress 要写入的从器件硬件地址，最后一位为读/写位
//			DataAddress  要写入的存储单元开始地址
//			pbuf		 指向数据缓存区的指针
//			Len			 写入数据长度
//返回值:	0->成功   1->失败
//**************************************************
bit WrToRomPageA(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len)
{
	u8 i = 0;
	IIC_Start();							// 启动总线
	if(IIC_SendByte(SlaveAddress&0xfe) == 1)// 写命令，已包含有应答函数
		return 1;
	if(IIC_SendByte(DataAddress) == 1)		// 已包含有应答函数
		return 1;
	for(i = 0; i < Len; i++)
	{
		if(IIC_SendByte(*pbuf++) == 1)		// 单片机向从机发送1个字节数据
			return 1;
	}
	IIC_Stop();								// 结束总线
	return 0;								// 写入多字节成功
}


//**************************************************
//功能描述: 多字节写入，完全不考虑芯片分页问题，速度慢
//			（写入时间大约是字节数n x 10ms）
//返回值:	0->成功   1->失败
//**************************************************
bit WrToRomA(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len)
{
	u8 i = 0;
	while(Len--)
	{
		if(WrToRomPageA(SlaveAddress, DataAddress++, pbuf++, 0x01) != 0)//写入一个字节
			return 1;						// 单字节写失败，程序返回
		SDA = 1;							// 判忙处理
		
		for(i = 0; i < 10; i++)				// 写入最长时间不超过10ms
		{
			DelayMs(1);
			IIC_Start();					// 启动总线
			if(IIC_SendByte(SlaveAddress&0xfe) == 0)
				break;
		}
	}	
	return 0;								// 成功返回0	
}


//**************************************************
//功能描述: 内部函数，页写，为跨页编写
//返回值:	0xff->失败
//**************************************************
u8 WrToRomPageB(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len)
{
	u8 i = 0;
	IIC_Start();							// 启动总线
	if(IIC_SendByte(SlaveAddress&0xfe) == 1)// 写命令，已包含有应答函数
		return 0xff;
	if(IIC_SendByte(DataAddress) == 1)		// 已包含有应答函数
		return 0xff;
	for(i = 0; i < Len;)
	{
		if(IIC_SendByte(*pbuf++) == 1)		// 单片机向从机发送1个字节数据
			return 0xff;
		i++;								// i++放到这里，一旦break退出当前循环，i++就不能完成本次加1而出错
		DataAddress++;
		if((DataAddress&0x07) == 0)			// 页越界。24C01/02每页8字节，每页起始地址低3位都是000
			break;							// 24C04/08/16每页16字节，0x07应改为0x0f
	}
	IIC_Stop();								// 结束总线
	
	return (Len-i);							// 写入多字节成功
}


//**************************************************
//功能描述: 多字节写入，不受页面大小限制，速度快
//返回值:	0->成功   1->失败
//**************************************************
bit WrToRomB(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len)
{
	u8 temp = Len;
	do{
		temp = WrToRomPageB(SlaveAddress, DataAddress+(Len-temp), pbuf+(Len-temp), temp);
		if(temp == 0xff)
			return 1;						// 失败返回1
		DelayMs(10);
	}while(temp);
	
	return 0;								// 成功返回0	
}


//**************************************************
//功能描述: 连续读操作，从DataAddress地址开始连续读取Len个字节到pbuf中
//参数说明: SlaveAddress 要读取的从器件硬件地址，最后一位为读/写位
//			DataAddress  要读取的存储单元开始地址
//			pbuf		 指向数据缓存区的指针
//			Len			 读取数据长度
//返回值:	0->成功   1->失败
//**************************************************
bit RdFromROM(u8 SlaveAddress, u8 DataAddress, u8 * pbuf, u8 Len)
{
	u8 i = 0;
	IIC_Start();							// 启动总线
	if(IIC_SendByte(SlaveAddress&0xfe) == 1)// 写命令，已包含有应答函数
		return 1;
	if(IIC_SendByte(DataAddress) == 1)		// 已包含有应答函数
		return 1;
	IIC_Start();							// 重新启动总线
	if(IIC_SendByte(SlaveAddress|0x01) == 1)// 读命令
		return 1;
	for(i = 0; i < Len-1; i++)
	{
		*pbuf++ = IIC_RecByte();			// 接收1个字节数据
		IIC_Ack();							// 应答0，告诉器件还要读下一字节数据
	}
	*pbuf = IIC_RecByte();					// 接收最后1个字节数据
	IIC_NoAck();							// 应答1，告诉器件不再读取数据
	IIC_Stop();								// 结束总线
	return 0;								// 读取多字节成功
}


//**************************************************
//功能描述: 写1个字节的数据到指定的地址
//参数说明: T_Data		待写的数据
//			RomAddress	指定的地址
//返回值:	无
//**************************************************
void write_I2C(uchar T_Data, uchar RomAddress)
{
	uchar W_Data = T_Data;
	WrToRomA(EE_DEV_ADDR | I2C_WR, RomAddress, &W_Data, 1);
}


//**************************************************
//功能描述: 从指定的地址读1个字节的数据
//参数说明: RomAddress	指定的地址
//返回值:	无
//**************************************************
uchar read_I2C(uchar RomAddress)
{
	uchar R_Data;

	RdFromROM(EE_DEV_ADDR, RomAddress, &R_Data, 1);
	
	return  R_Data;	
}


//**************************************************
//功能描述: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
//参数说明: SlaveAddress
//返回值:	0->成功   1->失败,未探测到
//**************************************************
u8 i2c_CheckDevice(u8 SlaveAddress)
{
	u8 ucAck;

	DelayMs(5);
	
	IIC_Start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	ucAck = IIC_SendByte(SlaveAddress|0x01);	/* 检测设备的ACK应答 */
	IIC_Stop();			/* 发送停止信号 */

	return ucAck;
}


//**************************************************
//功能描述: 判断串行EERPOM是否正常
//返回值:	0->正常   1->不正常
//**************************************************
u8 ee_CheckOk(void)
{
	if (i2c_CheckDevice(EE_DEV_ADDR) == 0)
	{
		return 0;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		IIC_Stop();		
		return 1;
	}
}


