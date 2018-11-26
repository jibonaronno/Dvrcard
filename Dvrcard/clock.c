/*
 * clock.c
 *
 * Created: 7/29/2013 1:53:28 AM
 *  Author: aronno
 */ 
#include "clock.h"
#include "ds1307_soft_i2c.h"
#include "global.h"

unsigned char ClockInit()
{
	//Initialize DS1307 Interface
	DS1307Init();

	//Clear CH bit of RTC
	#define CH 7

	uint8_t temp;
	if(!DS1307Read(0x00,&temp)) return 0;

	//Clear CH Bit
	temp&=(~(1<<CH));

	if(!DS1307Write(0x00,temp)) return 0;

	//Set 12 Hour Mode
	if(!DS1307Read(0x02,&temp)) return 0;

	//Set 12Hour BIT
	//temp|=(0b01000000);
	
	//set 24Hour Mode BIT
	temp &= (0b10111111);

	//Write Back to DS1307
	if(!DS1307Write(0x02,temp)) return 0;

	return 1;

}
/***************************************

Get second

Parameters
----------
NONE

Return Value
------------
the "second" part of time.

****************************************/
uint8_t GetSecond()
{
	uint8_t sec,temp;

	//Read the Second Register
	DS1307Read(0x00,&temp);
	sec=(((temp & 0b01110000)>>4)*10)+(temp & 0b00001111);

	return sec;

	
}
/***************************************

Get minute

Parameters
----------
NONE

Return Value
------------
the "minute" part of time.

****************************************/

uint8_t GetMinute()
{
	uint8_t min,temp;

	//Read the Minute Register
	DS1307Read(0x01,&temp);
	min=(((temp & 0b01110000)>>4)*10)+(temp & 0b00001111);

	return min;
	
}

/***************************************

Get hour

Parameters
----------
NONE

Return Value
------------
the "hour" part of time.

****************************************/

uint8_t GetHour()
{
	uint8_t hr,temp;

	//Read the Hour Register
	DS1307Read(0x02,&temp);
	hr=(((temp & 0b00110000)>>4)*10)+(temp & 0b00001111);

	return hr;

}

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

uint8_t GetAmPm()
{
	uint8_t am_pm,temp;

	//Read the Hour Register
	DS1307Read(0x02,&temp);

	am_pm=(temp & 0b00100000)>>4;

	return am_pm;

}
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

uint8_t SetSecond(uint8_t sec)
{
	uint8_t temp,result;

	temp=((sec/10)<<4)|(sec%10);
	result=DS1307Write(0x00,temp);

	return result;
}

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

uint8_t SetMinute(uint8_t min)
{
	uint8_t temp,result;

	temp=((min/10)<<4)|(min%10);
	result=DS1307Write(0x01,temp);

	return result;
	
}

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

uint8_t SetHour(uint8_t hr)
{
	uint8_t temp,result; //am_pm;

	//am_pm=GetAmPm();

	temp=((hr/10)<<4)|(hr%10);
	//temp|=0b01000000; //12 Hr Mode
	temp &= 0b10111111; //24 Hr Mode
	
	/*
	if(am_pm)
	{
		temp|=0b00100000;
	}
	*/
	result=DS1307Write(0x02,temp);

	return result;
	
}

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

unsigned char SetAmPm(BOOL pm)
{
	uint8_t temp,result;

	DS1307Read(0x02,&temp);

	if(pm)
		temp|=0b00100000;//SET
	else
		temp&=0b11011111;//CLEAR

	result=DS1307Write(0x02,temp);

	return result;
			
}

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

uint8_t GetTimeString(char *Time)
{
	uint8_t data;

	if(!DS1307Read(0x00,&data))
	{
		//I/O Error
		return 0;
	}

	Time[11]='\0';
	Time[10]='M';
	Time[8]=' ';

	Time[7]=48+(data & 0b00001111);
	Time[6]=48+((data & 0b01110000)>>4);
	Time[5]=':';

	if(!DS1307Read(0x01,&data))
	{
		//I/O Error
		return 0;
	}
	
	Time[4]=48+(data & 0b00001111);
	Time[3]=48+((data & 0b01110000)>>4);
	Time[2]=':';
	
	if(!DS1307Read(0x02,&data))
	{
		//I/O Error
		return 0;
	}
	
	Time[1]=48+(data & 0b00001111);
	Time[0]=48+((data & 0b00010000)>>4);

	//AM/PM
	if(data & 0b00100000)
	{
		Time[9]='P';
	}
	else
	{
		Time[9]='A';
	}

	return 1;//Success

}

