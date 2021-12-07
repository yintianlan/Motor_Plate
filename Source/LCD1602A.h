#ifndef _STD_DRV_1602_H_
#define _STD_DRV_1602_H_

#define uint unsigned int
#define uchar unsigned char
#define uint8 unsigned char


extern void lcd_set_posit(void);
extern void lcd_init(void);
extern void lcd_print(uint x, uint y, char string[17]);
extern void lcd_print2(uint x, uint y, char string[17]);

void LCD_Show_Test(void);

#endif

