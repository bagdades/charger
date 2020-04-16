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

#include "main.h"

#define REF_VOLT		5.01
#define DIV				4

uint16_t volt_temp, amp_temp;
uint8_t OCR_temp, calib_mode, batt, cc_mode, cv_mode, li_ion=1;//to denote li_ion as default battery type

void PWM_Init(void)
{
	/* TCCR2|=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS20); */
	/* DDRB|=(1<<3); */
	/* OCR2=0; */
}

uint8_t SwPressed(void)
{
	if (bit_is_clear(SW_PIN, PB4)) 
	{
		_delay_ms(25);
		if (bit_is_clear(SW_PIN, PB4)) 
			while(bit_is_clear(SW_PIN, PB4));
		if(calib_mode == 0)
			calib_mode = 1;
		else 
			calib_mode = 0;
		return 1;
	}
	return 0;
}

uint16_t MeasureVolt(void)
{
	unsigned int adc_avg;
	unsigned int volt;
	float temp_val;
	adc_avg = ADC_ReadAvg(1, 101);
	temp_val = (adc_avg * REF_VOLT * DIV) / 1024;
	volt = (unsigned int)(temp_val * 100);
	return volt;
}

uint16_t MeasureAmp(void)
{
	/* float temp_val; */
	/* uint16_t result; */
	uint16_t amp = 0;
	return amp;
}

void PrintConst1(void)
{
	LcdGotoxy(1,1);
	LcdPrint("Vo-");
	LcdGotoxy(6,1);
	LcdData('.');
	LcdGotoxy(16,1);
	LcdData('%');
	LcdGotoxy(1,2);
	LcdPrint("Vb-");
	LcdGotoxy(10,2);
	LcdPrint("Ia-");
	LcdGotoxy(15,2);
	LcdData('.');
}

void PrintParam(uint8_t x1, uint8_t y1, uint16_t val2)
{
	LcdGotoxy(x1, y1);
	LcdData((val2 / 1000) + 48);
	val2 %= 1000;
	LcdData((val2 / 100) + 48);
	val2 %= 100;
	LcdGotoxy((x1 + 2), y1);
	LcdData('.');
	LcdData((val2 / 10) + 48);
}

void Print(uint8_t val)
{
	val %= 100;
	LcdData((val / 10) + 48);
	val %= 10;
	LcdData(val + 48);
}

void PrintDuty(uint8_t vall)
{
	uint16_t temp;
	temp = ((vall + 1) * 100) / 256;
	LcdGotoxy(14, 1);
	Print((uint8_t)temp);
}

void ChargeLiIon(void)
{
	PORTB &= ~(1 << 1);//off mosfet in order to read the battery
	_delay_ms(10);
	if (volt_temp == 0) 
	{
		LcdClear();
		LcdGotoxy(1, 1);
		LcdPrint(" Insert battery");
		batt = 0;
		while (batt == 0) 
		{
			volt_temp = MeasureVolt();
			_delay_ms(500);
			if (volt_temp > 0) 
			{
				batt = 1;
			}
		}
	}
	if ((volt_temp < 900) && (volt_temp > 0)) 
	{
		LcdClear();
		LcdGotoxy(1, 1);
		LcdPrint(" Faulty battery");
		li_ion = 0;
		batt = 0;
	}
	if (volt_temp > 900) 
	{
		batt = 1;
	}
	if (batt == 1) 
	{
		LcdClear();
		PrintConst1();
		cc_mode = 1;
		LcdGotoxy(9, 1);
		LcdPrint("CC");
		OCR2A = 63;
		PORTB |= (1 << 1);

		while (cc_mode == 1) 
		{
			amp_temp = MeasureAmp();
			/* if (amp_temp < 100)  */
			/* { */
			/* 	OCR2++; */
			/* } */
			/* else */
			/* { */
			/* 	OCR2--; */
			/* } */
			PrintParam(4, 1, volt_temp);
			PrintParam(13, 2, amp_temp);
			PrintDuty(OCR2A);
			_delay_ms(500);
			PORTB &= ~(1 << 1);
			_delay_ms(10);
			volt_temp = MeasureVolt();
			if (volt_temp > 1240) 
			{
				cc_mode = 0;
				PrintParam(4, 2, volt_temp);
				LcdGotoxy(9, 1);
				LcdPrint("  ");
			}
			else
			{
				PrintParam(4, 2, volt_temp);
				PORTB |= (1 << 1);
			}
		}
		cv_mode = 1;
		LcdGotoxy(9, 1);
		LcdPrint("CV");
		PORTB |= (1 << 1);
		while (cv_mode == 1) 
		{
			volt_temp = MeasureVolt();
			if (volt_temp < 1250) 
			{
				OCR2A++;
			}
			else
			{
				OCR2A--;
			}
			PrintParam(4, 1, volt_temp);
			PrintDuty(OCR2A);
			MeasureAmp();
			_delay_ms(1000);
			PORTB &= ~(1 << 1);
			_delay_ms(10);
			volt_temp = MeasureVolt();
			PrintParam(4, 2, volt_temp);
			if ((amp_temp < 19) && (volt_temp > 1250)) 
			{
				LcdClear();
				LcdGotoxy(1, 1);
				LcdPrint("Charge comlete");
				cv_mode = 0;
				batt = 0;
				li_ion = 0;
				PORTB |= (1 << 2);
				break;
			}
			else
			{
				PrintParam(13, 2, amp_temp);
				PORTB |= (1 << 1);
			}
		}
	}
}

void Calibrate(void)
{
	LcdClear();
	LcdGotoxy(1, 2);
	LcdPrint("cal mode");
	while (calib_mode) 
	{
		volt_temp = MeasureVolt();
		PrintParam(4, 1, volt_temp);
		_delay_ms(100);
		if (SwPressed()) 
		{
			LcdClear();
			PrintConst1();
			_delay_ms(100);
		}
	}
}

int main(void)
{
	SW_PIN &= ~(1 << PB4); /* make PB0 input */
	SW_PORT |= (1 << PB4); /* pull up enable */
	DDRB |= (1 << PB1) | (1 << PB2);//make PB1,PB2 output to drive "output" mosfet & buzzer
	PORTB &= ~((1 << PB1) | (1 << PB2));//make it initially off i.e no buck output voltage to battery & buzzer off
	ADC_Init();
	_delay_ms(1);
	PWM_Init();
	_delay_ms(1);
	LcdInit();
	_delay_ms(10);
	/* OCR2 = 63; //(25% PWM output) */
	LcdGotoxy(1, 2);
	LcdPrint("Hello");
	while (1) 
	{
		if (SwPressed()) 
		{
			Calibrate();
		}
		if (li_ion) 
		{
			ChargeLiIon();
		}
	}
}

