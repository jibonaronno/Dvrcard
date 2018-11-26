
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <string.h>
#include "ButtonMatrix.h"
#include "Logger.h"
#include "lcdx.h"
#include "setupsettings.h"

uint8_t StrLen = 0;
char	chUival[8];
static uint8_t btnid=0;
static uint16_t PreValueU16 = 0;

uint16_t SetAndShowU16(uint16_t *iVal, char *chStr)
{
	StrLen = strlen(chStr);
	
	lcdClear();
	
	PreValueU16 = (*iVal);
	
	while (1)
	{
		lcdGotoXY(0, 1);
		GetNDigitFromShort(*iVal, chUival, 4);
		lcdPrintData(chStr, StrLen);
		lcdPrintData(chUival, 4);
		
		btnid = GetPressedButtonId();
		if (btnid > 0)
		{
			if (btnid == 3)
			{
				if ((*iVal) < 32768)
				{
					(*iVal)++;
				}
			}
			else if (btnid == 4)
			{
				if (*iVal > 1)
				{
					(*iVal)--;
				}
			}
			else if (btnid == 1)
			{
				(*iVal) = PreValueU16;
				return 0;
			}
			else if (btnid == 2)
			{
				return 0;
			}
		}
		
		_delay_ms(200);
		wdt_reset();
		
	}
	
	return 0;
	
}

