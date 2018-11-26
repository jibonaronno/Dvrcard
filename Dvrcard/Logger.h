/*
 * Logger.h
 *
 * Created: 10/27/2016 2:51:32 AM
 *  Author: Gabrial
 */ 
#include "rtclock.h"

#ifndef LOGGER_H_
#define LOGGER_H_

#define		YES				1
#define		NO				0
#define		SW_UP			PA0
#define		SW_DOWN			PA1
#define		SW_MENU			PA2
#define		SW_REG_CARD		PA3

#define		PIN_SW			PINA
#define		DDR_SW			DDRA
#define		PORT_SW			PORTA
#define		MENU_DELAY		30000
#define		DEB_SUCCESS		1
#define		DEB_LONG 		2
#define		DEB_FAILED 		0

#define INIT_IO		cbi(DDRA, PA4);sbi(PORTA, PA4);cbi(DDRA, PA5);sbi(PORTA, PA5);cbi(DDRA, PA6);sbi(PORTA, PA6);cbi(DDRA, PA7);sbi(PORTA, PA7);\
cbi(PORTB, PB4);sbi(DDRB, PB4);sbi(PORTB, PB5);sbi(DDRB, PB5);sbi(PORTB, PB6);cbi(DDRB, PB6);sbi(PORTB, PB7);sbi(DDRB, PB7);\
cbi(DDRB, PB2);sbi(PORTB,PB2);cbi(DDRB, PB3);sbi(PORTB,PB3);\
cbi(PORTD, PD3);sbi(DDRD, PD3)
//cbi(DDRB, PB0);sbi(PORTB,PB0);cbi(DDRB, PB1);sbi(PORTB,PB1);
//sbi(DDRC, PC0);sbi(PORTC, PC0)
//sbi(DDRC, PC4);sbi(DDRC, PC3);sbi(DDRC, PC2);cbi(PORTC, PC4);cbi(PORTC, PC3);cbi(PORTC, PC2);\
//sbi(DDRC, PC7);sbi(PORTC, PC7);sbi(DDRC, PC6);sbi(PORTC, PC6);sbi(DDRC, PC5);cbi(PORTC, PC5)

#define RECORD_SIZE		31

typedef struct __SETTINGS
{
	unsigned short DataStoreInterval;
	unsigned short DataSendInterval;
	unsigned short DataLoggingEnable;
	unsigned short offset;
	unsigned short calib;
}SETTINGS;

typedef struct __RECORD
{
	TIMESTAMP datetime;
	uint8_t batt_volts[24];
	uint8_t current;
}RECORD;

typedef struct __BLOCK_LOCATION
{
	uint32_t blockno;
	uint16_t index;
}BLOCK_LOCATION;

typedef struct __RECORDTABLE_DEF
{
	uint8_t key01;
	uint8_t key02;
	uint8_t key03;
	uint8_t key04;
	uint32_t recordcount;
	uint32_t startblockno;
	uint16_t singlerecordsize;
}RECORDTABLE_DEF;

typedef struct __CALIBVALUES 
{
	uint16_t calib_volt_001;
	uint16_t calib_curr_001;
	uint16_t curr_adc_offset001;
}CALIBVALUES;

void GetNDigitFromShort(unsigned short sDat, char *ch, unsigned char dnum);

#endif /* LOGGER_H_ */