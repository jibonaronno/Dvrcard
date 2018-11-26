
//#include "clock.h"

#ifndef RTCLOCK_H_
#define RTCLOCK_H_

typedef struct __TIMESTAMP
{
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned char year;
	unsigned char month;
	unsigned char day;
}TIMESTAMP;

void TransformTime(unsigned char *sec, unsigned char *min, unsigned char *hour, unsigned char *year, unsigned char *month, unsigned char *day);

unsigned char WriteRtcByte(unsigned char uAddress, unsigned char uData);

void rtcwrite(unsigned char devaddr, unsigned char address, unsigned char data);

unsigned char ReadRtcByte(unsigned char uAddress);;

void ReadRtcI2cTime(TIMESTAMP *tsmp);

void SetupClock(TIMESTAMP tsmp);

#endif