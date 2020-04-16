#ifndef  MAIN_INC
#define  MAIN_INC

#include <avr/io.h>
#include <util/delay.h>
#include <util/delay.h>
#include "lcd.h"
#include "adc.h"

#define SW_DDR		DDRB
#define SW_PIN		PINB
#define SW_PORT		PORTB
#define SWITCH		PB4
#define MOSFET		PB1
#define BUZZER		PB2

#endif   /* ----- #ifndef MAIN_INC  ----- */
