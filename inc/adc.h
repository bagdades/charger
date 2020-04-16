#ifndef  ADC_INC
#define  ADC_INC

#include <avr/io.h>
#include <util/delay.h>

#define ADCREF_AREF 0X00 //ADC reference voltage is set externally via Aref pin 
#define ADCREF_AVCC 0x40 //REFS0=1,aref=avcc internally,put cap from avcc to agnd,aref to agnd,connect avcc to vcc

/* functions prototyps */
void ADC_Init(void);
uint16_t ADC_ReadRaw(uint8_t ch);
uint16_t ADC_ReadAvg(uint8_t ch, uint16_t sample);

#endif   /* ----- #ifndef ADC_INC  ----- */
