/************************************************************

Libray fuctions to access the popular DS1307 RTC IC with AVR
Microcontroller.

The libray has just two functions. One reads the register whose
address is provided. Other writes to the given register with
given data.Please see DS1307 Datasheet for more info on
the registers.

Requires eXtreme Electronics Low Level Soft I2C Libray.

PLEASE SEE WWW.EXTREMEELECTRONICS.CO.IN FOR DETAILED 
SCHEMATICS,USER GUIDE AND VIDOES.

COPYRIGHT (C) 2008-2012 EXTREME ELECTRONICS INDIA

************************************************************/

#include "i2csoft.h"

#ifndef _DS1307_SOFT_H_
#define _DS1307_SOFT_H_

#define BOOL uint8_t

#define DS1307_SLA_W 0XD0
#define DS1307_SLA_R 0XD1

#define TRUE 	1
#define FALSE 	0

//Public Functions

void DS1307Init(void);

/***************************************************

Function To Write Internal Registers of DS1307
---------------------------------------------

address : Address of the register
data: value to write.


Returns:
0= Failure
1= Success
***************************************************/
uint8_t DS1307Write(uint8_t address,uint8_t data);


/***************************************************

Function To Read Internal Registers of DS1307
---------------------------------------------

address : Address of the register
data: value of register is copied to this.


Returns:
0= Failure
1= Success
***************************************************/
uint8_t DS1307Read(uint8_t address,uint8_t *data);

/***************************************************

Function To Read Internal Registers of DS1307
---------------------------------------------

address : Address of the register
data: value of register is copied to this.


Returns:
0= Failure
1= Success
***************************************************/

uint8_t DS1307Read(uint8_t address,uint8_t *data);

/***************************************************

Function To Write Internal Registers of DS1307
---------------------------------------------

address : Address of the register
data: value to write.


Returns:
0= Failure
1= Success
***************************************************/

uint8_t DS1307Write(uint8_t address,uint8_t data);


#endif
