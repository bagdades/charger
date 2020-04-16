/*
 * =====================================================================================
 *
 *       Filename:  lcd.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  23.03.20 21:50:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vovan (), volodumurkoval0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "lcd.h"


void LcdInit(void)
{
	 LCD_DDR = 0xFF;
	 LCD_PRT &= ~(1 << LCD_EN);
	 _delay_us(2000);
	 LcdCommand(0x33);
	 _delay_us(100);
	 LcdCommand(0x32);
	 _delay_us(100);
	 LcdCommand(0x28);
	 _delay_us(100);
	 LcdCommand(0x0c);
	 _delay_us(100);
	 LcdCommand(0x01);
	 _delay_us(2000);
	 LcdCommand(0x06);
	 _delay_us(100);
}

void LcdCommand(unsigned char command)
{
	LCD_PRT = (LCD_PRT & 0x0F) | (command & 0xF0);
	LCD_PRT &= ~ (1 << LCD_RS);
	LCD_PRT &= ~ (1 << LCD_RW); 
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_PRT &= ~ (1 << LCD_EN);
	_delay_us(20);
	LCD_PRT = (LCD_PRT & 0x0F) | (command << 4);
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);
}

void LcdData(unsigned char data)
{
	LCD_PRT = (LCD_PRT & 0x0F) | (data & 0xF0);
	LCD_PRT |= (1 << LCD_RS);
	LCD_PRT &= ~(1 << LCD_RW);
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);
	LCD_PRT = (LCD_PRT & 0x0F) | (data << 4);
	LCD_PRT |= (1 << LCD_RS);
	LCD_PRT &= ~(1 << LCD_RW);
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);         
	_delay_us(100);
}

void LcdGotoxy(unsigned char x, unsigned char y)
{
	unsigned char FIRSTCHARADDR[] = {0x80,0xc0,0x94,0xd4};// four line's address for 20x4 lcd
	LcdCommand(FIRSTCHARADDR[y-1]+x-1);
	_delay_us(100);
}

void LcdPrint(char*str)
{
	unsigned char i=0;
	while(str[i] != 0)
	{ 
		LcdData(str[i]);
		i++;
	}
}

void LcdClear(void)
{
	LcdCommand(0x01);
	_delay_ms(10);
}
