/*
 * =====================================================================================
 *
 *       Filename:  adc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02.04.20 21:51:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vovan (), volodumurkoval0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "adc.h"

void ADC_Init(void)
{
	DDRC = 0x00;          /* make adc port input */
	ADCSRA |=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//ADC enabled,clk/128 frequency
}

uint16_t ADC_ReadRaw(uint8_t ch)
{
	ch = ch & 0x07;
	ADMUX = ADCREF_AREF | ch;
	ADCSRA |= (1 << ADSC); /* adc start conversion */
	while((ADCSRA & (1 << ADIF)) == 0); /* stay until ADIF becomes 1 to indicate finish conversion */
	_delay_us(1);
	return(ADC);          /* return ADCH + ADCL  */
}

uint16_t ADC_ReadAvg(uint8_t ch, uint16_t sample)
{
	unsigned char i;
	unsigned int r[sample];//to store multiple reading.No of samples is defined by the user as per criteria
	unsigned int avg;//to store the average result
	unsigned long int sum = 0;//to store the sums of the readings and must be initialized as 0 to prevent errors
	for (i = 0; i < sample; i++) {
		r[i] = ADC_ReadRaw(ch);
		sum += r[i];
		_delay_ms(1);
	}
	avg = sum/sample;
	return avg;
}
