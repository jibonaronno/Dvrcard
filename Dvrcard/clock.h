/******************************************************************************

                          eXtreme Electronics xAPI(TM)
						  ----------------------------
xAPI is a Powerful but easy to use C library to program the xBoard(TM)
series of AVR development board. The library has support for commonly use tasks
like:-

*LCD interfacing
*MUXED 7 segment displays.
*Remote Control
*Serial Communication
*DC Motor Controls with Speed Control
*Analog Measurement for Sensor Interface.
*Temperature Measurement.
*I2C Communication.
*EEPROM Interface
*Real Time Clock (RTC Interface)

The APIs are highly documented and easy to use even by a beginner.

For More Info Log On to 
www.eXtremeElectronics.co.in

Copyright 2008-2009 eXtreme Electronics India

                                    Clock Core
						           ----------
This module is used for interfacing with DS1307 based clock module.
The library provide easy to use to use functions to get and set time.

For More information please see supplied tutorials and videos.

                                     NOTICE
									--------
NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN 
COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.


WRITTEN BY:
AVINASH GUPTA
me@avinashgupta.com

*******************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "ds1307_soft_i2c.h"

/***************************************

Setup the DS1307 Chip. Start it and set
12 Hr Mode.
Must be called before any other clock
related functions.

Parameters
----------
NONE

Return Value
------------
0=Failed
1=Success

****************************************/
unsigned char ClockInit();

/***************************************

Get second

Parameters
----------
NONE

Return Value
------------
the "second" part of time.

****************************************/
uint8_t GetSecond();

/***************************************

Get minute

Parameters
----------
NONE

Return Value
------------
the "minute" part of time.

****************************************/

uint8_t GetMinute();


/***************************************

Get hour

Parameters
----------
NONE

Return Value
------------
the "hour" part of time.

****************************************/

uint8_t GetHour();


/***************************************

Get am/pm

Parameters
----------
NONE

Return Value
------------
0=am
1=pm

****************************************/

uint8_t GetAmPm();

/***************************************

Set the second

Parameters
----------
seconds

Return Value
------------
0=failure
1=success

****************************************/

uint8_t SetSecond(uint8_t sec);


/***************************************

Set the minutes

Parameters
----------
minutes

Return Value
------------
0=failure
1=success

****************************************/

uint8_t SetMinute(uint8_t min);


/***************************************

Set the hour

Parameters
----------
hour

Return Value
------------
0=failure
1=success

****************************************/

uint8_t SetHour(uint8_t hr);


/***************************************

Set the second

Parameters
----------
0=am
1=pm

Return Value
------------
0=failure
1=success

****************************************/

unsigned char SetAmPm(BOOL pm);


/***************************************

Gets the current time as a ascii/text string.

Example 12:49:22 PM (HH:MM:SS: PM)


Parameters
----------
Pointer to a string.

Return Value
------------
0=failure
1=success

Example Usage
-------------

char time[12];			//The string must be at least 12bytes long
GetTimeString(time);	//Now the string time contains the current time


****************************************/

uint8_t GetTimeString(char *Time);




