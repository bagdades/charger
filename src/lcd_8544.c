/*
 * lcd_8544.c
 *
 *  Created on: 28 ����. 2010
 *      Author: vovan
 */

#include "lcd_8544.h"

uint8_t x_pos;
uint8_t y_pos;

static void Delay ( void )
{
    int i;

    for ( i = -32000; i < 32000; i++ );
}

/*
 * Name         :  LcdInit
 * Description  :  Performs MCU SPI & LCD controller initialization.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdInit(void){
#ifdef SPI_MODE
	SPIInit();
	LCD_PORT |= (1 << LCD_RST_PIN);
	LCD_DDR |= (1 << LCD_DC_PIN) | (1 << LCD_CE_PIN) | (1 << LCD_RST_PIN); /* All output */
#else
	LCD_PORT |= (1 << LCD_RST_PIN);
	LCD_DDR |= (1 << LCD_DC_PIN) | (1 << LCD_IN_PIN) | (1 << LCD_CLK_PIN) | (1 << LCD_CE_PIN) | (1 << LCD_RST_PIN); /* All output */

#endif
    /* Pull-up on reset pin. */
	LCD_PORT &= ~(1 << LCD_RST_PIN);
	Delay();
	/* _delay_ms(1); */
    LCD_PORT |= (1 << LCD_RST_PIN);

   /* Disable display controller. */
   LCD_PORT |= (1 << LCD_CE_PIN);
    LcdSend( 0x21, LCD_CMD ); /* LCD Extended Commands. */
    LcdSend( 0xC8, LCD_CMD ); /* Set LCD Vop (Contrast).*/
    LcdSend( 0x06, LCD_CMD ); /* Set Temp coefficent. */
    LcdSend( 0x13, LCD_CMD ); /* LCD bias mode 1:48. */
    LcdSend( 0x20, LCD_CMD ); /* LCD Standard Commands,Horizontal addressing mode */
    LcdSend( 0x0C, LCD_CMD ); /* LCD in normal mode. */

    LcdClear();
}

/*
 * Name         :  LcdSend
 * Description  :  Sends data to display controller.
 * Argument(s)  :  data -> Data to be sent
 *                 cd   -> Command or data (see enum in lcd8544.h)
 * Return value :  None.
 */
void LcdSend(uint8_t data, LcdCmdData cd){
#ifndef SPI_MODE
	 int bitcnt;
#endif
   /*  Enable display controller (active low). */
   LCD_PORT &= ~(1 << LCD_CE_PIN);
   if ( cd == LCD_DATA )
   {
	   LCD_PORT |= (1 << LCD_DC_PIN);
   }
   else
   {
	   LCD_PORT &= ~(1 << LCD_DC_PIN);
   }
#ifdef SPI_MODE
   SPDR = data;
   while(!(SPSR & (1 << SPIF)));
#else
	for (bitcnt = 8; bitcnt > 0; bitcnt--)
		{
			LCD_PORT &= ~(1 << LCD_CLK_PIN); // Set Clock Idle level LOW.
			if ((data & 0x80) == 0x80) 
				{LCD_PORT |= (1 << LCD_IN_PIN);}		// PCD8544 clocks in the MSb first.
			else 
				{LCD_PORT &= ~(1 << LCD_IN_PIN);}
			LCD_PORT |= (1 << LCD_CLK_PIN); // Data is clocked on the rising edge of SCK.
			data = data << 1;						// Logical shift data by 1 bit left.
		}
#endif
   /* Disable display controller. */
   LCD_PORT |= (1 << LCD_CE_PIN);
}

/*
 * Name         :  LcdClear
 * Description  :  Clear display
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdClear(void){
	uint16_t i;
	LcdGotoXY(LCD_CHAR,0,0);
	for (i = 0; i < LCD_SIZE; ++i) {
		LcdSend(0x00,LCD_DATA);
	}
}
/*
 * Name			: LcdGotoXY
 * Description	: Set cursor location to xy location
 * Argument (s)	: x,y-> Coordinate for new cursor position . Range :0,0 ... 13,5
 * Return value	: see return value in lcd_8544.
 */
uint8_t LcdGotoXY(LcdPixelChar mode ,uint8_t x,uint8_t y){
	if(mode == LCD_CHAR)
		x *= 6;
	x_pos = x;
	y_pos = y;
    if( x > LCD_X_RES)
        return OUT_OF_BORDER;
    if( y > LCD_Y_MAX)
        return OUT_OF_BORDER;
    LcdSend(0x80 | x,LCD_CMD);
    LcdSend(0x40 | y,LCD_CMD);
    return OK;

}
/*
 * Name         :  LcdChr
 * Description  :  Displays a character at current cursor location and
 *                 increment cursor location.
 * Argument(s)  :  size -> Font size. See enum in pcd8544.h.
 * 				   mode -> Mode inverse or normal. See enum in lcd8544.h
 *                 ch   -> Character to write.
 * Return value :  see lcd8544.h about return value
 */
uint8_t LcdChr ( LcdMode mode, uint8_t ch ){
	uint8_t temp;
	uint8_t i;

    if ( (ch < 0x20) || (ch > 0xFF) )
    {
        /* Convert to a printable character. */
        ch = 92;
    }
    if(mode == LCD_MODE_INVERSE){
    	LcdSend(0xFF,LCD_DATA);
    }
        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to LcdCache */
            temp = ( FontLookup[ ch - 32 ][ i ] ) << 1;
            if(mode == LCD_MODE_NORM)
            LcdSend(temp,LCD_DATA);
            else if(mode == LCD_MODE_INVERSE)
            	LcdSend(temp ^ 0xFF,LCD_DATA);
        }
    if(mode == LCD_MODE_NORM)
     LcdSend(0x00,LCD_DATA);
     else if(mode == LCD_MODE_INVERSE)
     	LcdSend(0xFF,LCD_DATA);//������� �� �������
    return OK;
}
/*
 * Name         :  LcdFStr
 * Description  :  Displays a character at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in SRAM
 * Argument(s)  :  mode -> Mode inverse or normal. See enum in lcd8544.h
 *                 startInverse -> number of charester
 *                 endInverse -> number of charester
 *                 data -> Pointer of string of char to be written
 * Return value :  count charester
 */
uint8_t LcdFStr (LcdMode mode,uint8_t startInverse,uint8_t endInverse, char* data )
{
	uint8_t out_mode;
	uint8_t ch = 1;
    while( *data != '\0' )
	{
    	if(mode == LCD_MODE_NORM || mode == LCD_MODE_INVERSE)
    		out_mode = mode;
    	else if(ch >= startInverse && ch <= endInverse)
    		out_mode = LCD_MODE_INVERSE;
    	else out_mode = LCD_MODE_NORM;
		LcdChr(out_mode, *data);
		data++;
		ch++;
	}
    return ch -= 1;
}

/*
 * Name         :  LcdStr
 * Description  :  Displays a character at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in SRAM
 * Argument(s)  :  mode -> Mode inverse or normal. See enum in lcd8544.h
 *                 startInverse -> number of charester
 *                 endInverse -> number of charester
 *                 data -> Pointer of string of char to be written
 * Return value :  count charester
 */
uint8_t LcdStr(LcdMode mode,uint8_t startInverse,uint8_t endInverse, uint8_t* data ){
	uint8_t out_mode;
	uint8_t ch = 1;
    while( *data != '\0' )
	{
    	if(mode == LCD_MODE_NORM || mode == LCD_MODE_INVERSE)
    		out_mode = mode;
    	else if(ch >= startInverse && ch <= endInverse)
    		out_mode = LCD_MODE_INVERSE;
    	else out_mode = LCD_MODE_NORM;
		LcdChr(out_mode, *data);
		data++;
		ch++;
	}
    return ch -= 1;
}
/*
 * Name         :  LcdContrast
 * Description  :  Set display contrast.
 * Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.
 * Return value :  None.
 */
//void LcdContrast ( uint8_t contrast )
//{
//    /* LCD Extended Commands. */
//    LcdSend( 0x21, LCD_CMD );
//
//    /* Set LCD contrast level. */
//    LcdSend( 0x80 | contrast, LCD_CMD );
//
//    /* LCD Standard Commands, horizontal addressing mode. */
//    LcdSend( 0x20, LCD_CMD );
//}
/*
 * Name			:	LcdZone
 * Description	:	Draw zone in lcd line x
 * Argument (s)	:	start_x -> coordinate start position (in pixel)
 * 					start_y -> number of start line
 * 					end_y -> number of end line
 * 					ch -> out char
 * 					length -> length zone (in pixel)
 * Return value	:	see return value in lcd8544.
 */
uint8_t LcdZone(uint8_t start_x,uint8_t start_y,uint8_t end_y,uint8_t ch,uint8_t length){
	uint8_t i,j;
	if( start_x > LCD_X_RES)
	        return OUT_OF_BORDER;
    if( start_y > LCD_Y_MAX)
        return OUT_OF_BORDER;
    for(j = start_y ; j <= end_y ; j++){
    	LcdSend(0x80 | start_x,LCD_CMD);
        LcdSend(0x40 | j,LCD_CMD);
		for (i = 0; i < length; ++i) {
			LcdSend(ch,LCD_DATA);
		}
    }
    return OK;
}
/*
 * Name			: LcdBigFontChar
 * Description	: Display a charester in current cursor location and
 * 				  increment cursor location
 * Argument (s)	: ch -> Charester to write
 * Return value	: See return value in lcd_8544.h
 */
uint8_t LcdBigFontChar(uint8_t ch)
{
	/* uint8_t i; */
	/* uint8_t temp; */
    /* if(ch == ':') */
    /* 	ch = 10; */
    /* else if(ch == '+') */
    /* 	ch = 11; */
    /* else if(ch == '-') */
    /* 	ch = 12; */
    /* else if(ch == '.') */
    /* 	ch = 13; */
    /* else if(ch == '=') */
    /* 	ch = 14; */
    /* else if(ch == 'A') */
    /* 	ch = 15; */
    /* else if(ch == 'I') */
    /* 	ch = 16; */
    /* else if(ch == 'U') */
    /*     ch = 17; */
    /* else if(ch == 'V') */
    /*     ch = 18; */
    /* else if(ch < '0' || ch > '9') */
    /* 	ch = 0; */
    /* else ch-= '0'; */
    /* for (i = 0; i < 18; i+=2) { */
	/* 	temp = bigFont[ch ][i]; */
	/* 	LcdSend(temp ,LCD_DATA); */
	/* } */
    /* LcdSend(0x00,LCD_DATA); */
    /* LcdSend(0x80 | x_pos,LCD_CMD);//���������� �� ������� ������� � ������ ������� */
    /* LcdSend(0x40 | (y_pos - 1),LCD_CMD); */
    /* for (i = 1; i < 18; i+=2) { */
    /* 		temp = bigFont[ch ][i]; */
    /* 		LcdSend(temp ,LCD_DATA); */
    /* 	} */
    /* LcdSend(0x00,LCD_DATA); */
    /* x_pos += 10;			//����������� ������� "�" ���������� ������� */
    /* if(x_pos >= LCD_X_RES - 10){//���� ���� ��� ���������� ������� ������ */
    /* 	y_pos +=2;				//���������� � ��������� ����� */
    /* 	x_pos = 1; */
    /* } */
    /* LcdSend(0x80 | x_pos,LCD_CMD); */
    /* LcdSend(0x40 | y_pos,LCD_CMD); */
    return OK;
}
/*
 * Name			: LcdBigFStr
 * Description	: Display a string in current cursor location and
 * 				  increment cursor location
 * Argument (s)	: ch -> Pointer of string to write
 * Return value	: See return value in lcd_8544.h
 */
uint8_t LcdBigFStr(char *ch){
	while(*ch){
		LcdBigFontChar(*ch);
		ch++;
	}
	return OK;
}

/*
 * Name			: LcdBigStr
 * Description	: Display a string in current cursor location and
 * 				  increment cursor location
 * Argument (s)	: ch -> Pointer of string to write
 * Return value	: See return value in lcd_8544.h
 */
uint8_t LcdBigStr(uint8_t *ch){
	while(*ch){
		LcdBigFontChar(*ch);
		ch++;
	}
	return OK;
}

void SPIInit(void)
{
	LCD_DDR |= (1 << LCD_IN_PIN) | (1 << LCD_CLK_PIN);
	SPCR = (1 << SPE) | (1 << MSTR);
}
