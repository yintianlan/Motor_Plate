#include<STC15F2K60S2.h>
#include <INTRINS.H>

#include <string.h> 
#include <stdlib.h>
#include <stdio.h>

#include "LCD1602A.h"


#define MIN(x, y) ((x) < (y) ? (x):(y))
#define MAX(x, y) ((x) > (y) ? (x):(y))

#define PIN_ENABLE		1
#define PIN_DISABLE		0
#define BTN_ENABLE		0
#define BTN_DISABLE		1

sbit EN = P0^7;
sbit RS = P0^5;
sbit RW = P0^6;
sbit E = P0^7; 

#define DataPort P6//LCD Port

#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1


static uint8 lcd_y=0;
static uint8 lcd_x=0;


#if 1//20191126//
extern void DelayMs(unsigned int n);

/*------------------------------------------------
              LCD 延时函数
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{
	while(--t);
}

/*------------------------------------------------
              LCD 检查忙函数
------------------------------------------------*/
bit LCD_Check_Busy(void)
{
	DataPort= 0xFF;
	RS_CLR;
	RW_SET;
	EN_CLR;
	_nop_();
	EN_SET;
	return (bit)(DataPort & 0x80);
}

/*------------------------------------------------
              LCD 写命令函数
------------------------------------------------*/
void LCD_Write_Com(unsigned char com)
{
	//while(LCD_Check_Busy()); //??????ò????????
	//RS_CLR;
	//RW_CLR;
	//EN_SET;
	DataPort= com;
	RS_CLR;
	RW_CLR;

	EN_CLR;
	DelayMs(1);
	EN_SET;
	DelayMs(1);
	EN_CLR;
}

/*------------------------------------------------
              LCD 写数据函数
------------------------------------------------*/
void LCD_Write_Data(unsigned char Data)
{
	//while(LCD_Check_Busy()); //??????ò????????
	//RS_SET; 
	//RW_CLR; 
	//EN_SET; 
	DataPort= Data;
	RS_SET;
	RW_CLR;
	EN_CLR;
	DelayMs(1);
	EN_SET;
	DelayMs(1);
	EN_CLR;
}

/*------------------------------------------------
                LCD 清屏函数
------------------------------------------------*/
void LCD_Clear(void)
{
	LCD_Write_Com(0x01);
	DelayMs(5);
}

/*------------------------------------------------
              LCD 写一个数据串
------------------------------------------------*/
void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s)
{
	if (y == 0) 
	{
		LCD_Write_Com(0x80 + x);
	}
	else 
	{
		LCD_Write_Com(0xC0 + x);
	}
	while (*s)
	{
		LCD_Write_Data( *s);
		s ++;
	}
}

/*------------------------------------------------
              LCD 写一个数据
------------------------------------------------*/
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data)
{
	if (y == 0)
	{
		LCD_Write_Com(0x80 + x);
	}
	else
	{
		LCD_Write_Com(0xC0 + x);
	}
	LCD_Write_Data( Data);
}

/*------------------------------------------------
              设置显示 RAM 起始地址，
		亦即光标位置，(x,y)-对应屏幕上的字符坐标
------------------------------------------------*/
void LCD_Set_Cursor(unsigned char x, unsigned char y)
{
    unsigned char addr;
    if (y == 0) { //由输入的屏幕坐标计算显示 RAM 的地址
        addr = 0x00 + x; //第一行字符地址从 0x00 起始
    } else {
        addr = 0x40 + x; //第二行字符地址从 0x40 起始
    }
    LCD_Write_Com(addr | 0x80); //设置 RAM 地址
}
/*------------------------------------------------
------------------------------------------------*/
/* lcd 显示字符函数 */
/* 一行最多显示16个字符，这里为17是因为最后一个字符为'\0'*/
void lcd_print2(uint x, uint y, char string[17])
{
	LCD_Write_String(x,y,string);
}

/* lcd 显示字符函数 */
void lcd_print3(char* str,int len)
{
	int i=0;
	static uint8 y=0;
	static uint8 x=0;
	
	for(i=0;i<len;i++)
	{
		LCD_Write_Char(x++%16,y%2,str[i]);
		
		if(x%16==0)
		{
			y++;
		}

		DelayMs(250);
	}
}

/* lcd 显示字符函数 */
void lcd_print4(char dat)
{
	int i=0;
	
	//for(i=0;i<len;i++)
	{
		LCD_Write_Char(lcd_x++%16, lcd_y%2, dat);
		
		if(lcd_x%16==0)
		{
			lcd_y++;
		}
		//DelayMs(250);
	}
}

/* LCD 设置原点*/
void lcd_set_posit(void)
{
	lcd_y=0;
	lcd_x=0;
}

/* LCD 初始化*/
void LCD_Init2(void)
{
	RS_CLR;
	RW_CLR;
	EN_CLR;

	LCD_Write_Com(0x38);//BASIC SETTINGS 基本设置
	LCD_Write_Com(0x0C);//EDIT NOMARK 显示设置
	LCD_Write_Com(0x06);//MOVE RNEXT 输入方式从左到右
	LCD_Write_Com(0x01);//ECHO CLEAR 清屏
}

/* lcd1602 初始化 */
void lcd_init(void)
{
	LCD_Init2();//初始化屏幕
	LCD_Set_Cursor(0, 0);//设置光标原点
}

void lcd_main(void)
{
	unsigned char i;
	unsigned char *p;
//	unsigned char j;

	LCD_Init2();

	//for(j=0;j<100;j++)//while (1) 
	{
		i = 1;
		p = "https://www.sovitt.com";

		//lcd_print3(p ,strlen(p));
	}
}

#else
sbit LCD_RS = P0^5;
sbit LCD_RW = P0^6;
sbit LCD_EP = P0^7;

//sbit btn_1 = P2^1;
//sbit btn_2 = P2^2;
//sbit btn_3 = P2^3;
//sbit btn_4 = P2^4;
//sbit beep = P2^0;

#define STD_BUFFER_LENGTH 65

#define LCD_DATA P1

#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1


//* lcd controls
#define LCD_LINE_LENGTH_MAX	16

#define LCD_BUSY_BIT_MASK	0x80
#define LCD_BASIC_SETTINGS	0x38

#define LCD_EDIT_NOMARK		0x0C
#define LCD_EDIT_MARK		0x0F

#define LCD_MOVE_RNEXT		0x06
#define LCD_ECHO_CLEAR		0x01

#define LCD_ECHO_LINE1		0x00
#define LCD_ECHO_LINE2		0x40


bit  lcd_busy_now(void);
void lcd_cmd(char cmd);
void lcd_set_pos(char pos);
void lcd_data(char byte);
void lcd_edit_mark(uchar pos);
void lcd_edit_done(void);
void lcd_set_posit(void);


void lcd_delay(uint i);

//* data[7] = 1: busy
//* data[7] = 0: ready
bit  lcd_busy_now(void)
{
	bit is_busy = 0; //return is_busy;

	LCD_DATA = 0x00;

	LCD_RS = PIN_DISABLE;
	LCD_RW = PIN_ENABLE;
	LCD_EP = PIN_ENABLE;

	lcd_delay(4);

	//is_busy = (P0 & 0x80) ? 1 : 0; //0x80
	is_busy = (bit) (LCD_DATA & LCD_BUSY_BIT_MASK); //0x80
	LCD_EP = PIN_DISABLE;

	return is_busy;
}

void lcd_cmd(char cmd)
{
	while(lcd_busy_now());

	LCD_RS = PIN_DISABLE;
	LCD_RW = PIN_DISABLE;
	LCD_EP = PIN_DISABLE;
	lcd_delay(2);

	LCD_DATA = cmd;
	lcd_delay(4);

	LCD_EP = PIN_ENABLE;
	lcd_delay(4);

	LCD_EP = PIN_DISABLE;
}

void lcd_set_pos(char pos)
{
	lcd_cmd(pos | LCD_BUSY_BIT_MASK); //0x80
}

void lcd_data(char byte)
{
	while(lcd_busy_now());

	LCD_RS = PIN_ENABLE;
	LCD_RW = PIN_DISABLE;
	LCD_EP = PIN_DISABLE;
	LCD_DATA = byte;
	lcd_delay(4);

	LCD_EP = PIN_ENABLE;
	lcd_delay(4);
	LCD_EP = PIN_DISABLE;
}

void lcd_init(void)
{
	lcd_cmd(LCD_BASIC_SETTINGS); // 0x38: total 16*2, 5*7 each, 8 bit data
	lcd_delay(1);

	lcd_cmd(LCD_EDIT_NOMARK); // 0x0C: ECHO on, no CURSE
	lcd_delay(1);

	lcd_cmd(LCD_MOVE_RNEXT); // 0x06: move curse to RIGHT NEXT
	lcd_delay(1);

	lcd_cmd(LCD_ECHO_CLEAR); // 0x01: clear
	lcd_delay(1);
}


void lcd_print(uint x, uint y, char string[17])
{
	unsigned int i;

	//uart_send(string);

	lcd_cmd(LCD_MOVE_RNEXT); // 0x06

	if (y == 1) {
		lcd_set_pos(LCD_ECHO_LINE1 + x);
	} else {
		lcd_set_pos(LCD_ECHO_LINE2 + x);
	}

	i = 0;
	while(string[i] != '\0' && i < LCD_LINE_LENGTH_MAX) {
		lcd_data(string[i]);
		i ++;
	}
}

void lcd_edit_mark(uchar pos)
{
	lcd_cmd(LCD_EDIT_MARK);
	if (pos) lcd_set_pos(pos);
}

void lcd_edit_done(void)
{
	lcd_cmd(LCD_EDIT_NOMARK);
}


void lcd_delay(uint i)
{
	while(i--)
		_nop_();
}

#endif


uchar code table[]={
0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,//一

0x00,0x00,0x0E,0x00,0x1F,0x00,0x00,0x00,//二

0x00,0x1F,0x00,0x0E,0x00,0x1F,0x00,0x00,//三

0x02,0x04,0x0F,0x12,0x0F,0x0A,0x1F,0x02,//年

0x0F,0x09,0x0F,0x09,0x0F,0x09,0x09,0x11,//月

0x1F,0x11,0x11,0x1F,0x11,0x11,0x1F,0x00,//日

0x00,0x00,0x00,0x0A,0x15,0x0A,0x04,0x00,//心型

0x00,0x04,0x15,0x0E,0x1F,0x0E,0x11,0x00//坦克

};

uchar code table1[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};  //自定义字符数据地址

uchar code table2[]="QQ:598852247";
//————————————————
//版权声明：本文为CSDN博主「张飞online」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/u013372900/article/details/51479327


void LCD_Show_Test(void)
{
	uchar i;
	
	//设定CGRAM地址
	LCD_Write_Com(0x40);    //开始写入你要显示的自定义字符、汉字代码
	for(i=0;i<64;i++)	//将自定义字型码写入CGRAM中，但是一般最多只能同时显示8个汉字
	{
		LCD_Write_Data(table[i]);
		DelayUs2x(5);
	}

	LCD_Write_Com(0x80);    //从第一行第一列开始显示
	for(i=0;i<8;i++)     //显示自定义字符
	{
		LCD_Write_Data(table1[i]);  
		DelayUs2x(5);
	}

	LCD_Write_Com(0xc0);   //显示QQ：598852247
	for(i=0;i<12;i++)
	{
		LCD_Write_Data(table2[i]);
		DelayUs2x(5);
	}
}
