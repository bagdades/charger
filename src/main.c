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
	TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1); 
	TCCR2B |= (1 << CS20);
	/* TCCR2B |= (1 << WGM22) | (1 << CS20); */
	DDRB |= (1 << PB3);
	OCR2A = 0;
}

uint8_t SwPressed(void)
{
	if (bit_is_clear(SW_PIN, SWITCH)) 
	{
		_delay_ms(25);
		if (bit_is_clear(SW_PIN, SWITCH)) 
			while(bit_is_clear(SW_PIN, SWITCH));
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
	float temp_val;
	uint16_t result;
	uint16_t amp = 0;
	result = ADC_ReadAvg(0, 101);
	temp_val = (result * REF_VOLT) / 1024;
	amp = (uint16_t)(temp_val * 200);
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
	PORTB &= ~(1 << MOSFET);//off mosfet in order to read the battery
	_delay_ms(10);
	volt_temp = MeasureVolt();
	if (volt_temp == 0) 
	{
		LcdClear();
		LcdGotoxy(1, 1);
		LcdPrint(" Insert battery");
		batt = 0;
		while (batt == 0) /* This loop is created if someone turns on the charger without battery adn then plugs in the battery */
		{
			volt_temp = MeasureVolt(); /* Check if battery detected */
			_delay_ms(500); /* after every 500 ms */
			if (volt_temp > 0) /* if battery voltage greater than 0v, then get out of the loop */
			{
				batt = 1;
			}
		}
	}
//------if battery detected,check the battery status--------//
	if ((volt_temp < 900) && (volt_temp > 0)) /* if batter voltage is below 9.00v and gteater than 0v */
	{
		LcdClear();
		LcdGotoxy(1, 1);
		LcdPrint(" Faulty battery"); /* notify that it is faulty battery */
		li_ion = 0;      /* exit the charging process */
		batt = 0;        /* - to notify a faulty battery */
	}
	if (volt_temp > 900) 
	{
		batt = 1;        /* proper battery detected, enter into charging mode */
	}
//--------if the battery is found ok,then-----------//
	if (batt == 1)       /* if the battery is connected */
	{
		LcdClear();
		PrintConst1();
		cc_mode = 1;     /* now enter in CC mode */
		LcdGotoxy(9, 1);
		LcdPrint("CC");  /* notify cc mode via lcd */
		OCR2A = 63;      /* start PWM form 25% */
		PORTB |= (1 << MOSFET); /* open the output mosfet */
//------now in CC mode----------//
		while (cc_mode == 1) 
		{
			amp_temp = MeasureAmp(); /* measure amp */
			if (amp_temp < 100) 
			{
				OCR2A++;
			}
			else
			{
				OCR2A--;
			}
			volt_temp = MeasureVolt(); /* Now measure the voltage */
			PrintParam(4, 1, volt_temp); /* print the output voltage */
			PrintParam(13, 2, amp_temp); /* print the current */
			PrintDuty(OCR2A);
			_delay_ms(500); /* wait 100 ms for next */
			PORTB &= ~(1 << MOSFET); /* now switch off the mosfet to measure battery voltage */
			_delay_ms(10); /* wait little hit */
			volt_temp = MeasureVolt();
			if (volt_temp > 1240) /* if the battery voltage is greater than 12.4 volt */
			{
				cc_mode = 0; /* exit CC mode */
				PrintParam(4, 2, volt_temp); /* print the voltage */
				LcdGotoxy(9, 1);
				LcdPrint("  "); /* notify that we have exited CC mode via lcd */
			}
			else
			{
               /* continue with CC mode */
				PrintParam(4, 2, volt_temp);
				PORTB |= (1 << MOSFET); /* again turn on the modfet */
			}
		}
//--------now entering CV mode---------//
		cv_mode = 1;
		LcdGotoxy(9, 1);
		LcdPrint("CV"); /* nofify CV mode via lcd */
		PORTB |= (1 << MOSFET); /* start charge */
		while (cv_mode == 1) 
		{
			volt_temp = MeasureVolt();
               /* we will maintain the output voltage at 12.6v as CV mode voltage */
			if (volt_temp < 1250) 
			{
				OCR2A++;
			}
			else
			{
				OCR2A--;
			}
			PrintParam(4, 1, volt_temp); /* Print the output voltage */
			PrintDuty(OCR2A);
			amp_temp = MeasureAmp();
			_delay_ms(1000);
			PORTB &= ~(1 << MOSFET); /* now switch off the mosfet to measure battery voltage */
			_delay_ms(10); /* wait a little hit */
			volt_temp = MeasureVolt();
			PrintParam(4, 2, volt_temp); /* print battery voltage */
			if ((amp_temp < 19) && (volt_temp > 1250)) /* if amp below approx 190ma & battery voltage is greater than 12.5v */
			{
				LcdClear();
				LcdGotoxy(1, 1);
				LcdPrint("Charge comlete");
				cv_mode = 0; /* stop cv mode and complete charge cycle */
				batt = 0;
				li_ion = 0;
				PORTB |= (1 << BUZZER); /* turn on buzzer to notify a full charge */
				break;
			}
			else
			{
				PrintParam(13, 2, amp_temp); /* print the current adn continue with CV mode */
				PORTB |= (1 << MOSFET); /* switch the mosfet on again */
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
	SW_PIN &= ~(1 << SWITCH); /* make PB0 input */
	SW_PORT |= (1 << SWITCH); /* pull up enable */
	DDRB |= (1 << MOSFET) | (1 << BUZZER);//make PB1,PB2 output to drive "output" mosfet & buzzer
	PORTB &= ~((1 << MOSFET) | (1 << BUZZER));//make it initially off i.e no buck output voltage to battery & buzzer off
	ADC_Init();
	_delay_ms(1);
	PWM_Init();
	_delay_ms(1);
	LcdInit();
	_delay_ms(10);
	OCR2A = 63; //(25% PWM output)
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

