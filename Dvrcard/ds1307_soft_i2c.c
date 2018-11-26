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

#include <avr/io.h>

#include "i2csoft.h"
#include "ds1307_soft_i2c.h"
#include "global.h"


void DS1307Init(void)
{
	SoftI2CInit();
}

uint8_t DS1307Read(uint8_t address,uint8_t *data)
{
	uint8_t res;	//result
	
	//Start
	SoftI2CStart();
	
	//SLA+W (for dummy write to set register pointer)
	res=SoftI2CWriteByte(DS1307_SLA_W);	//DS1307 address + W
	
	//Error
	if(!res)	return FALSE;
	
	//Now send the address of required register
	res=SoftI2CWriteByte(address);
	
	//Error
	if(!res)	return FALSE;
	
	//Repeat Start
	SoftI2CStart();
	
	//SLA + R
	res=SoftI2CWriteByte(DS1307_SLA_R);	//DS1307 Address + R
	
	//Error
	if(!res)	return FALSE;
	
	//Now read the value with NACK
	*data=SoftI2CReadByte(0);
	
	//Error
	if(!res)	return FALSE;
	
	//STOP
	SoftI2CStop();
	
	return TRUE;
}

uint8_t DS1307Write(uint8_t address,uint8_t data)
{
	uint8_t res;	//result
	
	//Start
	SoftI2CStart();
	
	//SLA+W
	res=SoftI2CWriteByte(DS1307_SLA_W);	//DS1307 address + W
	
	//Error
	if(!res)	return FALSE;
	
	//Now send the address of required register
	res=SoftI2CWriteByte(address);
	
	//Error
	if(!res)	return FALSE;
	
	//Now write the value
	res=SoftI2CWriteByte(data);
	
	//Error
	if(!res)	return FALSE;
	
	//STOP
	SoftI2CStop();
	
	return TRUE;
}

