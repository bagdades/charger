/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  22.03.20 16:07:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vovan (), volodumurkoval0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd_8544.h"

int main(void)
{
	DDRA |= (1 << 0);
	PORTA &= ~(1 << 0);
	LcdInit();
	LcdGotoXY(LCD_CHAR, 0, 0);
	LcdStr(LCD_MODE_NORM, 0, 0, (uint8_t*)"Hello!");
	LcdGotoXY(LCD_CHAR, 0, 1);
	LcdStr(LCD_MODE_NORM, 0, 0, (uint8_t*)"Hello!");
	LcdGotoXY(LCD_CHAR, 0, 2);
	LcdStr(LCD_MODE_NORM, 0, 0, (uint8_t*)"Hello!");
	LcdGotoXY(LCD_CHAR, 0, 3);
	LcdStr(LCD_MODE_NORM, 0, 0, (uint8_t*)"Hello!");
	LcdGotoXY(LCD_CHAR, 0, 4);
	LcdStr(LCD_MODE_NORM, 0, 0, (uint8_t*)"Hello!");
	while (1) 
	{
		PORTA |= (1 << 0);
		_delay_ms(500);
		PORTA &= ~(1 << 0);
		_delay_ms(500);
	}
}

