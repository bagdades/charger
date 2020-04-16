#ifndef  LCD_INC
#define  LCD_INC

#include <avr/io.h>
#include <util/delay.h>

#define LCD_PRT PORTD//define lcd port here for data & command pins
#define LCD_DDR DDRD
#define LCD_PIN PIND
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

/* function prototypes */

void LcdCommand(unsigned char);
void LcdData(unsigned char);
void LcdInit(void);
void LcdGotoxy(unsigned char, unsigned char);
void LcdPrint(char*str);
void LcdClear(void);

#endif   /* ----- #ifndef LCD_INC  ----- */
