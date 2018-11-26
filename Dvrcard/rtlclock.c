/*
 * rtlclock.c
 *
 * Created: 7/29/2013 1:57:32 AM
 *  Author: aronno
 */ 

#include "rtclock.h"
#include "ds1307_soft_i2c.h"
#include "global.h"
#include "clock.h"

void TransformTime(unsigned char *sec, unsigned char *min, unsigned char *hour, unsigned char *year, unsigned char *month, unsigned char *day)
{

  *sec  =  (((*sec & 0xF0) >> 4)*10) + (*sec & 0x0F);
  *min  =  (((*min & 0xF0) >> 4)*10) + (*min & 0x0F);
  *hour =  (((*hour & 0xF0) >> 4)*10) + (*hour & 0x0F);
//  *year =  (((*day & 0xC0)) >> 6);
//  *year =  (((*year & 0xF0)) >> 4);
  *day  =  ((*day & 0x30) >> 4)*10 + (*day & 0x0F);
  *month = ((*month & 0x10) >> 4)*10 + (*month & 0x0F);
  *year =  (((*year & 0xF0)) >> 4)*10 + (*year & 0x0F);
	
}

unsigned char WriteRtcByte(unsigned char uAddress, unsigned char uData)
{
	DS1307Write(uAddress, uData);

	return 1;
}

void rtcwrite(unsigned char devaddr, unsigned char address, unsigned char data)
{
	DS1307Write(address, data);
}

unsigned char ReadRtcByte(unsigned char uAddress)
{
	unsigned char uDat = 0;
	
	DS1307Read(uAddress, &uDat);

	return uDat;
	
}

void ReadRtcI2cTime(TIMESTAMP *tsmp)
{
	uint8_t sec = 0;
	uint8_t min = 0;
	uint8_t hour = 0;
	uint8_t month = 0;
	uint8_t year = 0;
	uint8_t day = 0;
	
	////<MOD>tsmp->sec = GetSecond();
	////<MOD>tsmp->min = GetMinute();
	////<MOD>tsmp->hour = GetHour();
	
	//tsmp->day = GetDa
	
	DS1307Read(0x00, &sec);
	DS1307Read(0x01, &min);
	DS1307Read(0x02, &hour);
	
	DS1307Read(0x04, &day);
	delay_ms(1);
	DS1307Read(0x05, &month);
	delay_ms(1);
	DS1307Read(0x06, &year);
	
	TransformTime(&sec, &min, &hour, &year, &month, &day);
	
	tsmp->sec = sec;
	tsmp->min = min;
	tsmp->hour = hour;
	
	tsmp->day = day;
	
	tsmp->month = month;
	
	tsmp->year = year;
	
}

void SetupClock(TIMESTAMP tsmp)
{

  /*
  *sec  =  (((*sec & 0xF0) >> 4)*10) + (*sec & 0x0F);
  *min  =  (((*min & 0xF0) >> 4)*10) + (*min & 0x0F);

  *hour =  (((*hour & 0xF0) >> 4)*10) + (*hour & 0x0F);

  *year =  (((*day & 0xC0)) >> 6);
  *day  =  ((*day & 0x30) >> 4)*10 + (*day & 0x0F);
  *month = ((*month & 0x10) >> 4)*10 + (*month & 0x0F);
  */
	
	unsigned char sec = (((tsmp.sec/10) << 4) | (tsmp.sec%10));
	unsigned char min = (((tsmp.min/10) << 4) | (tsmp.min%10));
	unsigned char hour = (((tsmp.hour/10) << 4) | (tsmp.hour%10));

	unsigned char day = (((tsmp.day/10) << 4) | (tsmp.day%10));

	unsigned char month = (((tsmp.month/10) << 4) | (tsmp.month%10));

	unsigned char year = (((tsmp.year/10) << 4) | (tsmp.year%10));
	
	DS1307Write(0x00, sec);
	DS1307Write(0x01, min);
	DS1307Write(0x02, hour);
	DS1307Write(0x04, day);
	DS1307Write(0x05, month);
	DS1307Write(0x06, year);

	/*
	WriteRtcByte(0x00, sec);
	WriteRtcByte(0x01, min);
	WriteRtcByte(0x02, hour);
	WriteRtcByte(0x04, day);
	WriteRtcByte(0x05, month);
	WriteRtcByte(0x06, year);
	*/
}

