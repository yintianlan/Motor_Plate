#ifndef __IIC_H_
#define __IIC_H_


void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NoAck(void);
bit IIC_GetACK(void);
unsigned char IIC_SendByte(unsigned char Data);
unsigned char IIC_RecByte(void);


#endif
