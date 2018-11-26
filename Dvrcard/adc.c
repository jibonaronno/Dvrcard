
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>
#include <stdint.h>
#include "global.h"
#include "adc.h"

uint16_t adc_main = 0;
uint16_t adc_channel1 = 0;
uint16_t adc_channel2 = 0;
uint8_t T2AdcTickCount = 0;
uint8_t AdcInt = 0;
uint8_t ad_l = 0;
uint8_t ad_h = 0;
uint16_t ad = 0;
uint8_t adsumcount = 0;
uint32_t adsum_x = 0;

void SetupAdc(unsigned char mode)
{
	if(mode == ADC_MODE_NOMRAL_FREE)
	{
		ADMUX = 0b01000001; // 01 - AVCC 5 V | 0 - ADCLAR(RIGHT) | 00001 - Single Ended Channel 1

		ADCSRA = 0b10001110; // 1 - ADEN | 0 - ADSC | 0 - ADATE | 0 - ADIF | 1 - ADIE | 001 - Prescaler Div/2

		SFIOR = (SFIOR & 0b00001111); // 000 - ADTS2:ADTS1:ADTS0 = FREE RUNNING | 0 = RESERVED | 1111 - ACME:PUD:PSR2:PSR0 = NOT NEEDED NOW ;;; 100 - ADTS2:ADTS1:ADTS0 = Timer0 Overflow

		sbi(ADCSRA, ADIE);

	}
}

void StartAdc()
{
	sbi(ADCSRA, ADSC);
}

void StopAdc()
{
	cbi(ADCSRA, ADSC);
}

ISR(ADC_vect)
{
	
	ad_l = ADCL;
	ad_h = ADCH;
	ad = 0;
//	ad = ad_l;
	ad |= (((unsigned short)ad_h)<<8);
	ad |= ((unsigned short)ad_l);
//	AdcInt = 1;
	
	if(adsumcount > 1)
	{
		//adc_main = adsum_x/40;
		// WE ARE TAKING 2ND SAMPLE FROM ADC.
		adc_main = ad;
		adsum_x = 0;
		adsumcount = 0;
		AdcInt = 1;
		
		if ((ADMUX & 0b00011111) == CHANNEL_1)
		{
			adc_channel1 = adc_main;
			ADMUX = ((ADMUX & 0b11100000) | CHANNEL_2); 
		}
		else if ((ADMUX & 0b00011111) == CHANNEL_2)
		{
			adc_channel2 = adc_main;
			ADMUX = ((ADMUX & 0b11100000) | CHANNEL_1);
		}
	}
	else
	{
		//adsum_x += ad;
		adsumcount++;
	}
	StartAdc();
}
