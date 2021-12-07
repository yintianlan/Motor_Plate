#include "IIC.H"

#include <INTRINS.H>
#include "GpioDefine.H"

//I2C传输延时函数
static void delay5us(void)		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 16;
	while (--i);
}

//起始位
void IIC_Start(void)
{
	SDA = 1;
	SCL = 1;
	delay5us();
	SDA = 0;
	delay5us();
	SCL = 0;
}

//停止位
void IIC_Stop(void)
{
	SDA = 0;
	SCL = 1;
	delay5us();
	SDA = 1;
	delay5us();
}

//主机应答位
void IIC_Ack(void)
{
	SDA = 0;
	delay5us();
	SCL = 1;
	delay5us();
	SCL = 0;
	delay5us();//此语句可以不要
}

//主机反向应答位
void IIC_NoAck(void)
{
	SDA = 1;
	delay5us();
	SCL = 1;
	delay5us();
	SCL = 0;
	delay5us();//此语句可以不要
}

//获取从机应答信号
// 0: 收到ACK
// 1: 没收到ACK
bit IIC_GetACK(void)
{
	bit ErrorBit;
	SDA = 1;
	delay5us();
	SCL = 1;
	delay5us();
	ErrorBit = SDA;
	SCL = 0;
	delay5us();//此语句可以不要
	
	return ErrorBit;
}	

//**************************************************
//功能描述: 主设备向从设备发送一个字节
//返回值:	0->成功   1->失败
//**************************************************
unsigned char IIC_SendByte(unsigned char Data)
{
	unsigned char i;		//位数控制
	for(i = 0; i < 8; i++)	//写入时是用时钟下降沿同步数据
	{
		if(Data & 0x80)
			SDA = 1;
		else
			SDA = 0;
		delay5us();
		SCL = 1;
		delay5us();
		SCL = 0;
		delay5us();
		Data = Data << 1;
	}
	return IIC_GetACK();
}

//**************************************************
//功能描述: 主设备向从设备读取一个字节
//返回值:	读取到的字节
//**************************************************
unsigned char IIC_RecByte(void)
{
	unsigned char i,rbyte=0;
	SDA = 1;
	for(i = 0; i < 8; i++)	//读出时是用时钟上升沿同步数据
	{
		SCL = 0;
		delay5us();
		SCL = 1;
		delay5us();
		if(SDA)
			rbyte |= (0x80 >> i);
	}
	SCL = 0;
	
	return rbyte;
}
