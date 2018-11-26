/*
 * ButtonMatrix.c
 *
 * Created: 7/24/2013 4:17:40 PM
 *  Author: aronno
 */ 
#include "ButtonMatrix.h"
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
//<MOD>#include "lcdx.h"

unsigned char ButtonStates;
uint8_t ActiveColumn = BTNMATRIX_COLUMN1;
uint8_t iCount=0;
uint16_t BtnPressCount = 0;

volatile uint8_t PressedButtonId = 0;
volatile uint8_t LongPress = 0;
volatile uint8_t Button = 0;
volatile uint8_t click = 0;
volatile uint8_t ButtonIdReadFlag = 0;
volatile uint16_t NextButtonDelayCount = 0;

uint16_t		SHORTPRESS_COUNT = 100;
uint16_t		LONGPRESS_COUNT = 1000;

uint8_t GetPressedButtonId()
{
	if (PressedButtonId > 0)
	{
		ButtonIdReadFlag = 1;
		return PressedButtonId;
	}	
	
	return 0;
}

uint8_t GetLongPressedButtonId()
{
	if ((LongPress == 1)&&(PressedButtonId > 0))
	{
		ButtonIdReadFlag = 1;
		return PressedButtonId;
	}
	
	return 0;
}

void ButtonMatrixPrimaryCycle()
{
	if(ActiveColumn == BTNMATRIX_COLUMN1)
	{
		CHECKPIN1
		{
			click = 1;
			if (Button != 1)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 1;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 1;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 1;
			}
		}
		else CHECKPIN2
		{
			click = 1;
			if (Button != 2)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 2;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 2;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 2;
			}
		}
		else CHECKPIN3
		{
			click = 1;
			if (Button != 3)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 3;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 3;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 3;
			}
		}
		else CHECKPIN4
		{
			click = 1;
			if (Button != 4)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 4;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 4;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 4;
			}
		}
		else
		{
			//<MOD>ACTIVE_COLUMN2;
			ActiveColumn = BTNMATRIX_COLUMN1; //<MOD>
		}
	}
	else if(ActiveColumn == BTNMATRIX_COLUMN2)
	{
		CHECKPIN1
		{
			click = 1;
			if (Button != 5)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 5;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 5;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 5;
			}
		}
		else CHECKPIN2
		{
			click = 1;
			if (Button != 6)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 6;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 6;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 6;
			}
		}
		else CHECKPIN3
		{
			click = 1;
			if (Button != 7)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 7;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 7;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 7;
			}
		}
		else CHECKPIN4
		{
			click = 1;
			if (Button != 8)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 8;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 8;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 8;
			}
		}
		else
		{
			//<MOD>ACTIVE_COLUMN3;
			//<MOD>ActiveColumn = BTNMATRIX_COLUMN3;
		}
	}
	else if(ActiveColumn == BTNMATRIX_COLUMN3)
	{
		CHECKPIN1
		{
			click = 1;
			if (Button != 9)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 9;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 9;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 9;
			}
		}
		else CHECKPIN2
		{
			click = 1;
			if (Button != 10)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 10;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 10;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 10;
			}
		}
		else CHECKPIN3
		{
			click = 1;
			if (Button != 11)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 11;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 11;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 11;
			}
		}
		else CHECKPIN4
		{
			click = 1;
			if (Button != 12)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 12;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 12;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 12;
			}
		}
		else
		{
			//<MOD>ACTIVE_COLUMN4;
			//<MOD>ActiveColumn = BTNMATRIX_COLUMN4;
		}
	}
	else if(ActiveColumn == BTNMATRIX_COLUMN4)
	{
		CHECKPIN1
		{
			click = 1;
			if (Button != 13)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 13;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 13;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 13;
			}
		}
		else CHECKPIN2
		{
			click = 1;
			if (Button != 14)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 14;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 14;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 14;
			}
		}
		else CHECKPIN3
		{
			click = 1;
			if (Button != 15)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 15;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 15;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 15;
			}
		}
		else CHECKPIN4
		{
			click = 1;
			if (Button != 16)
			{
				BtnPressCount = 0;
				PressedButtonId = 0;
				LongPress = 0;
				Button = 16;
			}
			BtnPressCount++;
			if (BtnPressCount > LONGPRESS_COUNT)
			{
				PressedButtonId = 16;
				BtnPressCount = LONGPRESS_COUNT + 2;
				LongPress = 1;
			}
			else if (BtnPressCount > SHORTPRESS_COUNT)
			{
				PressedButtonId = 16;
			}
		}
		else
		{
			//<MOD>ACTIVE_COLUMN1;
			//<MOD>ActiveColumn = BTNMATRIX_COLUMN1;
		}
	}
	
	if (ButtonIdReadFlag == 1)
	{
		PressedButtonId = 0;
		NextButtonDelayCount++;
		
		if (NextButtonDelayCount > 500)
		{
			NextButtonDelayCount = 0;
			ButtonIdReadFlag = 0;
		}
	}
	
	if (click != 1)
	{
		BtnPressCount = 0;
		PressedButtonId = 0;
		LongPress = 0;
		Button = 0;
	}
	
	click = 0;
}

