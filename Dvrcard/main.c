//***********************************************************
// **** MAIN routine FOR Interfacing microSD/SDHC CARD ****
//***********************************************************
//Controller: ATmega32 (Clock: 8 Mhz-internal)
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Project V.: Version - 2.4.1
//Author	: CC Dharmani, Chennai (India)
//			  www.dharmanitech.com
//Date		: 24 Apr 2011
//***********************************************************

//Link to the Post: http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html

#define F_CPU 16000000UL		//freq 8 MHz
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
#include "SPI_routines.h"
#include "SD_routines.h"
#include "UART_routines.h"
#include "RTC_routines.h"
#include "i2c_routines.h"
#include "Logger.h"
#include "global.h"
#include "adc.h"
#include "usart.h"
#include "clock.h"
#include "ds1307_soft_i2c.h"
#include "rtclock.h"
#include "lcdx.h"
#include "ButtonMatrix.h"
#include "MCP2303soft.h"
#include "setupsettings.h"

volatile unsigned short SecondTickCount = 0;
volatile unsigned char  SecondTickInt = 0;
volatile uint8_t PrimarySecCount = 0;

unsigned char T2TickInt003 = 0;
unsigned short T2TickCount = 0;
uint16_t T2TickCount002 = 0;
uint8_t  T2TickInt002 = 0;

//extern volatile uint8_t PressedButtonId;
extern volatile uint8_t rxComplete; //from usart.c
uint8_t rxSize=0;
char chuart01[22];

char ch_txData[250];
uint8_t txDataNumBuffer[60];

char chLocalRecord[33];
char chRtable[22];
char chData01[32];
char chRecCount[8];

extern uint16_t adc_main;
extern uint16_t adc_channel1;
extern uint16_t adc_channel2;

uint16_t mcp_adc_channel01 = 0;
uint16_t mcp_adc_channel02 = 0;
uint16_t mcp_adc_channel03 = 0;

volatile uint16_t mcp_adc01_sum = 0;
volatile uint8_t mcp_adc01_acq_count = 0;
volatile uint16_t mcp_adc01_temp = 0;

volatile uint16_t mcp_adc02_sum = 0;
volatile uint16_t mcp_adc_temp = 0;
volatile uint8_t mcp_adc02_acq_count = 0;

volatile uint16_t mcp_temp2 = 0;

volatile uint32_t sec_counter = 0;

uint16_t volt001 = 0;
uint16_t curr001 = 0;

TIMESTAMP tstamp;

RECORDTABLE_DEF rtable;

CALIBVALUES calibvalues;

RECORD local_record;

/************************************************************************/
/* Energy Calculation Variables.                                        */
/************************************************************************/
volatile uint8_t  UnitIntigratorTickInt = 0;
uint32_t PresentIntigratedUnits = 0;
uint16_t PresentIntigratedMicroUnits = 0;
uint16_t PresentIntigratedMiliUnits = 0;
uint16_t templ1 = 0;
uint16_t templ2 = 0;
uint32_t DevideFactor = 3600000l;
uint32_t wattPerSec = 0;

/************************************************************************/
/* </>Energy Calculation Variables.                                     */
/************************************************************************/

void port_init(void)
{
PORTA = 0x00;
DDRA  = 0x00;
PORTB = 0xEF;
DDRB  = 0xBF; //MISO line i/p, rest o/p
PORTC = 0x00;
DDRC  = 0x00;
PORTD = 0x00;
DDRD  = 0xFE;
}


//call this routine to initialize all peripherals
void init_devices(void)
{
	unsigned char tmr2ctrl = 0;
	cli();  //all interrupts disabled
	//port_init();
	INIT_IO;
	//spi_init();
	//<RTC>ClockInit();
	//twi_init();
	//uart0_init();
	USART_Init(95);
	
	/*****************************TIMER2 STARTUP CODE***************************/
	/*<CODE> TIMER2 STARTUP CODE. ENABLING INTERRUPT. NO PWM. ClockIO/32 Speed*/
	TIMSK = 0b01000000;    //Enable Interrupt for Timer2
	tmr2ctrl = 0b00000011; //Bit[6]:Bit[3] - WGM20:WGM21 = 00 PWM Mode Disable | Bit[5]:Bit[4] - COM21:COM20 = 00| Bit[2]:Bit[1]:Bit[0] - CS02:CS01:CS00 = 011 CLOCKio/32
	//76543210
	TCCR2 = tmr2ctrl;
	/************************************</CODE>********************************/
	
	//sei();

	// MCUCR = 0x00;
	// GICR  = 0x00;
	// TIMSK = 0x00; //timer interrupt sources
	//all peripherals are now initialized
}

ISR(TIMER2_OVF_vect)
{
	T2TickCount++;
	SecondTickCount++;
	
	T2TickCount002++;
	
	RxWatchdogCountUpCycle();
	
	ButtonMatrixPrimaryCycle();
	
	if (SecondTickCount > 50)  //976) //1/2 Second tick at 16MHz
	{
		SecondTickCount = 0;
		SecondTickInt = 1;
	}
	
	if(T2TickCount >= 1952)
	{
		T2TickInt003 = 1;
		T2TickCount = 0;
		UnitIntigratorTickInt = 1;
		
		sec_counter++;
	}
	
	if(T2TickCount002 > 400)
	{
		T2TickInt002 = 1;
		T2TickCount002 = 0;
	}
}

uint16_t CRC16cal(char *buffer, uint16_t size)
{
	uint16_t i=0;
	uint16_t j=0;
	uint16_t CRC = 0xFFFF;
	uint8_t  CRCHi = 0xFF;
	uint8_t  CRCLo = 0xFF;
	uint8_t  CRCLSB = 0x00;
	for (i=0; i < size; i++)
	{
		CRC = CRC ^ buffer[i];
		for (j=0; j < 8; j++)
		{
			CRCLSB = (CRC & 0x0001);
			CRC = ((CRC >> 1) & 0x7FFF);
			if (CRCLSB)
			{
				CRC = (CRC ^ 0xA001);
			}
		}
	}
	return CRC;
}


void GetNDigitFromShort(unsigned short sDat, char *ch, unsigned char dnum)
{
	char cc[20];
	unsigned char help = 0;
	unsigned char numofsteps = 0;
	unsigned char initindex = 0;
	uint8_t len = 0;
	uint8_t i = 0;
	unsigned char j = 0;
	itoa(sDat, cc, 10);
	len = strlen((const char*)cc);
	for(i=0;i<dnum;i++)
	{
		ch[i] = '0';
	}

	if(len >= dnum)
	{
		help = dnum;

		initindex = (len - dnum);

		for(i=0;i<help;i++)
		{
			ch[i] = cc[initindex+i];
		}
	}
	else
	{
		initindex = dnum - len;

		numofsteps = len;

		for(i=initindex;i<(initindex+numofsteps);i++)
		{
			ch[i] = cc[j];

			j++;
			
		}
	}

	ch[dnum] = '\0';
}

void GetNDigitFromu32(uint32_t sDat, char *ch, uint8_t dnum)
{
	char cc[20];
	unsigned char help = 0;
	unsigned char numofsteps = 0;
	unsigned char initindex = 0;
	uint8_t len = 0;
	uint8_t i = 0;
	unsigned char j = 0;
	itoa(sDat, cc, 10);
	len = strlen((const char*)cc);
	for(i=0;i<dnum;i++)
	{
		ch[i] = '0';
	}

	if(len >= dnum)
	{
		help = dnum;

		initindex = (len - dnum);

		for(i=0;i<help;i++)
		{
			ch[i] = cc[initindex+i];
		}
	}
	else
	{
		initindex = dnum - len;

		numofsteps = len;

		for(i=initindex;i<(initindex+numofsteps);i++)
		{
			ch[i] = cc[j];

			j++;
			
		}
	}

	ch[dnum] = '\0';
}

void ShowDateTime(TIMESTAMP tsmp, uint8_t lcdx, uint8_t lcdy)
{
	char cVal[10];

	lcdGotoXY(lcdx, lcdy);

	GetNDigitFromShort(tsmp.hour, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));

	lcdPrintData(":", 1);
	
	GetNDigitFromShort(tsmp.min, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));

	lcdPrintData((char*)":", 1);

	GetNDigitFromShort(tsmp.sec, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));
	lcdPrintData(" ", 1);
}

void ShowDate(TIMESTAMP tsmp, u08 lcdx, u08 lcdy)
{
	char cVal[10];
	lcdGotoXY(lcdx, lcdy);

	GetNDigitFromShort(tsmp.day, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));
	lcdPrintData("/", 1);
	GetNDigitFromShort(tsmp.month, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));
	lcdPrintData((char*)"/", 1);
	GetNDigitFromShort(tsmp.year, cVal, 2);
	lcdPrintData((char*)cVal, strlen((const char*)cVal));
	lcdPrintData(" ", 1);
}

void InitializeRtable(void)
{
	uint16_t local_idx=0;
	SD_readSingleBlock(0UL);
	memcpy((void *)&rtable, (const void *)buffer, sizeof(RECORDTABLE_DEF));
	
	if (rtable.key01 == 0x32 && rtable.key02 == 0x55 && rtable.key03 == 0x73 && rtable.key04 == 0x3F)
	{
		;
	}
	else
	{
		for (local_idx=0;local_idx<512;local_idx++)
		{
			buffer[local_idx]=0x1F;
		}
		rtable.key01 = 0x32;
		rtable.key02 = 0x55; 
		rtable.key03 = 0x73; 
		rtable.key04 = 0x3F;
		rtable.recordcount = 0UL;
		rtable.startblockno = 1UL;
		rtable.singlerecordsize = RECORD_SIZE;
		memcpy((void *)buffer, (const void*)&rtable, sizeof(RECORDTABLE_DEF));
		SD_writeSingleBlock(0UL);
	}
}

void StoreBuffer(unsigned char *buffer, uint8_t _size)
{
	eeprom_write_block ((void *)buffer, (void *)10, _size);
}

void RestoreBuffer(unsigned char *buffer, uint8_t _size)
{
	eeprom_read_block ((void *)buffer, (void *)10, _size);
}

void StoreEnergyReading(char *buffer, uint8_t _size)
{
	eeprom_write_block ((void *)buffer, (void *)30, _size);
}

void ReStoreEnergyReading(char *buffer, uint8_t _size)
{
	eeprom_read_block ((void *)buffer, (void *)30, _size);
}

void GetNextBlockLocationToWrite(BLOCK_LOCATION *bloc)
{
	uint32_t tmpblockno=1UL;
	uint32_t modulas01=0UL;
	
	if ((rtable.recordcount * RECORD_SIZE) > 512)
	{
		tmpblockno = (rtable.recordcount * RECORD_SIZE) / 512;
		modulas01 = (rtable.recordcount * RECORD_SIZE) % 512;
		if ((512-modulas01) > RECORD_SIZE)
		{
			bloc->blockno = tmpblockno+1UL;
			bloc->index = (uint16_t)modulas01;
		}
		else
		{
			bloc->blockno = (tmpblockno + 2);
			bloc->index = 0;
		}
	}
	else
	{
		if ((512-(rtable.recordcount * RECORD_SIZE)) > RECORD_SIZE)
		{
			bloc->blockno=1UL;
			bloc->index = rtable.recordcount * RECORD_SIZE;
		}
		else
		{
			bloc->blockno = 2UL;
			bloc->index = 0;
		}
	}
}

void GetNextBlockLocationToWrite2(BLOCK_LOCATION *bloc)
{
	//uint32_t tmpblockno=1UL;
	//uint32_t modulas01=0UL;
	uint32_t modulas02=0UL;
	
	modulas02 = rtable.recordcount % 16;
	
	if (modulas02 >= 15)
	{
		bloc->blockno = (rtable.recordcount / 16) + 2;
		bloc->index = 0;
	}
	else
	{
		bloc->blockno = (rtable.recordcount / 16) + 1;
		
		bloc->index = modulas02 * 31;
	}
}

void GetNextBlockLocationFromPresentBloc(BLOCK_LOCATION *bloc)
{
	if ((bloc->blockno > 0) && (bloc->blockno < 9999))
	{
		if (bloc->index > 0)
		{
			if ((512 - bloc->index) > RECORD_SIZE)
			{
				bloc->index += RECORD_SIZE;
			}
			else
			{
				bloc->blockno += 1;
				bloc->index = 0;
			}
		}
		else
		{
			bloc->index += RECORD_SIZE;
		}
	}
}

uint8_t ReadRecordFromBuffer(RECORD *record, BLOCK_LOCATION *bloc)
{
	memcpy((void *)record, (const void *)&buffer[bloc->index], RECORD_SIZE);
	return 0;
}

void ReadRecordsAndTransmit(int16_t count, uint32_t blockno)
{
	BLOCK_LOCATION bloc;
	uint16_t local_idx=0;
	uint16_t buffer_idx = 0;
	if (count <= 16)
	{
		if ((blockno < 9999)&&(blockno > 0))
		{
			bloc.blockno = blockno;
			bloc.index=0;
			cli();
			SD_readSingleBlock(blockno);
			
			for (local_idx = 0; local_idx < count; local_idx++)
			{
				memcpy((void *)&local_record, (const void *)&buffer[buffer_idx], RECORD_SIZE);
				sprintf(chLocalRecord, "%02d:%02d:%02d %02d/%02d/%02d curr:%d\r\n", local_record.datetime.hour, local_record.datetime.min, local_record.datetime.sec, local_record.datetime.day, local_record.datetime.month, local_record.datetime.year, local_record.current);
				transmitString(chLocalRecord);
				buffer_idx += RECORD_SIZE;
			}
			
		}
	}
	sei();
}

void ReadRecordsAndTransmitAll(void)
{
	uint16_t modulas01=0;
	uint32_t local_idx=0UL;
	uint32_t local_idx01=0UL;
	uint32_t block_count = 0;
	char chCount[6];
	
	block_count = rtable.recordcount / 16;
	
	modulas01 = rtable.recordcount % 16;
	
	for (local_idx=0;local_idx < block_count; local_idx++)
	{
		GetNDigitFromShort(local_idx, chCount, 4);
		transmitString("BLOCK:");
		transmitString(chCount);
		transmitString("\r\n");
		ReadRecordsAndTransmit(15, local_idx);
		_delay_ms(200);
	}
}

void StoreRtable(void)
{
	uint16_t local_idx=0;
	for (local_idx=0;local_idx<512;local_idx++)
	{
		buffer[local_idx]=0x1F;
	}
	memcpy((void *)buffer, (const void*)&rtable, sizeof(RECORDTABLE_DEF));
	cli();
	SD_writeSingleBlock(0UL);
	_delay_ms(100);
	sei();
}

void IncreamentRecordCount(void)
{
	if (rtable.recordcount < 9999)
	{
		rtable.recordcount++;
	}
	StoreRtable();
}

void WriteRecord(RECORD *rec)
{
	BLOCK_LOCATION bloc;
	uint16_t local_idx=0;
	GetNextBlockLocationToWrite2(&bloc);
	sprintf(chData01, "BLOCKNO:%d IDX:%d DX:%d\r\n", bloc.blockno, (uint32_t)bloc.index, bloc.index);
	//GetNDigitFromShort(bloc.index,  chData01, 4);
	transmitString(chData01);
	cli();
	if (bloc.index == 0)
	{
		for (local_idx=0;local_idx<512;local_idx++)
		{
			buffer[local_idx] = 0xFF;
		}
	}
	else
	{
		SD_readSingleBlock(bloc.blockno);
	}

	memcpy((void *)&buffer[bloc.index], (const void *)rec, RECORD_SIZE);
	SD_writeSingleBlock(bloc.blockno);
	IncreamentRecordCount();
	
	_delay_ms(100);
	
	sei();
}

void ReadRtableAndTransmit(void)
{
	uint16_t local_idx=0;
	cli();
	SD_readSingleBlock(0UL);
	_delay_ms(100);
	sei();
	memcpy((void *)&rtable, (const void *)buffer, sizeof(RECORDTABLE_DEF));
	sprintf(chRtable, "Record Count:%d\r\n", rtable.recordcount);
	transmitString(chRtable);
}

void ResetRecordCount(void)
{
	rtable.recordcount = 0;
	StoreRtable();
}

SETTINGS setting;


uint32_t GetLastUnit(void)
{
	uint32_t Dat = 0;
	ReStoreEnergyReading((char *)&Dat, 4);
	return Dat;
}

void SetLastUnit(uint32_t Dat)
{
	StoreEnergyReading((char *)&Dat, 4);
}

void UpdateEnergyCalculation(void)
{
	uint32_t curr;
	uint32_t vol;
	
	if (UnitIntigratorTickInt == 1)
	{
		UnitIntigratorTickInt = 0;
				
		curr = (uint32_t)curr001;
		vol = (uint32_t)volt001;
				
		//wattPerSec = 247u;//(current * volt);
		if(curr > 1)
		{
			wattPerSec = (curr * vol)/100;
		}
		else
		{
			wattPerSec = 0;
		}
				
		if (wattPerSec >= 36u)
		{
			//PresentIntigratedUnits = ((wattPerSec * 10l)/36l);//(wattPerSec * 1000000l)/3600000l;
			templ1 = (wattPerSec * 10u) / 36u;

			PresentIntigratedMicroUnits += templ1;
			if(PresentIntigratedMicroUnits > 10000l)
			{
				templ2 = (PresentIntigratedMicroUnits / 10000l);
				PresentIntigratedMicroUnits -= (templ2 * 10000l);
				PresentIntigratedMiliUnits += templ2;
				
				//if (PresentIntigratedMiliUnits >= 100)
				//{
					//PresentIntigratedUnits += 1l;
					//PresentIntigratedMiliUnits = 0;
				//}
				
				//eeprom_write_word((const uint16_t*)40, PresentIntigratedMiliUnits);
				
			}
		}
	}
}

void SetupTimeStamp(unsigned char *strSetting, TIMESTAMP *tsmp)
{
	char cHour[5];
	char cMin[5];
	char cSec[5];
	uint8_t btnid=0;
	
	GetNDigitFromShort(tsmp->hour, cHour, 2);
	GetNDigitFromShort(tsmp->min, cMin, 2);
	GetNDigitFromShort(tsmp->sec, cSec, 2);
	lcdClear();
	
	lcdGotoXY(0,0);
	lcdPrintData((char*)strSetting, strlen((const char*)strSetting));
	
	ShowDateTime(*tsmp, 10, 0);
	lcdGotoXY(10,1);
	lcdPrintData("^^", 2);
	
	while (1)
	{
		btnid = GetPressedButtonId();

		if (btnid > 0)
		{
			if (btnid == 2)
			{
				if (tsmp->hour < 24)
				{
					tsmp->hour++;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(10,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 3)
			{
				if(tsmp->hour > 0)
				{
					tsmp->hour--;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(10,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(13,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
		wdt_reset();
	}
	
	while(1)
	{
		btnid = GetPressedButtonId();

		if (btnid > 0)
		{
			if (btnid == 2)
			{
				if (tsmp->min < 60)
				{
					tsmp->min++;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(13,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 3)
			{
				if(tsmp->min > 0)
				{
					tsmp->min--;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(13,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(16,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
		wdt_reset();
	}
	
	while(1)
	{
		btnid = GetPressedButtonId();

		if (btnid > 0)
		{
			if (btnid == 2)
			{
				if (tsmp->sec < 60)
				{
					tsmp->sec++;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(16,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 3)
			{
				if(tsmp->sec > 0)
				{
					tsmp->sec--;
					ShowDateTime(*tsmp, 10, 0);
					lcdGotoXY(16,1);
					lcdPrintData("^^", 2);
				}
				_delay_ms(50);
			}
			
			if (btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(16,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
		wdt_reset();
	}
	
}


void SetupDateStamp(unsigned char *strSetting, TIMESTAMP *tsmp)
{
	char cDay[8];
	char cMonth[8];
	char cYear[8];
	
	uint8_t btnid = 0;

	GetNDigitFromShort(tsmp->day, cDay, 2);
	GetNDigitFromShort(tsmp->month, cMonth, 2);
	GetNDigitFromShort(tsmp->year, cYear, 2);
	lcdGotoXY(0,0);
	lcdPrintData("                    ", 20);
	lcdGotoXY(0,0);
	lcdPrintData((char*)strSetting, strlen((const char*)strSetting));
	ShowDate(*tsmp, 10, 0);
	lcdGotoXY(10,1);
	lcdPrintData("^^", 2);
	
	while(1)
	{
		wdt_reset();

		ShowDate(*tsmp, 10, 0);
		lcdGotoXY(10,1);
		lcdPrintData("^^", 2);
		
		btnid = GetPressedButtonId();
		
		if(btnid > 0)
		{
			if (btnid == 2)
			{
				if(tsmp->day < 31)
				{
					tsmp->day++;
				}
				_delay_ms(50);
			}
			
			if(btnid == 3)
			{
				if(tsmp->day > 0)
				{
					tsmp->day--;
				}
				_delay_ms(50);
			}
			
			if(btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(13,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
	}

	while(1)
	{
		wdt_reset();

		ShowDate(*tsmp, 10, 0);
		lcdGotoXY(13,1);
		lcdPrintData("^^", 2);
		
		btnid = GetPressedButtonId();
		
		if(btnid > 0)
		{
			if (btnid == 2)
			{
				if(tsmp->month < 12)
				{
					tsmp->month++;
				}
				_delay_ms(50);
			}
			
			if(btnid == 3)
			{
				if(tsmp->month > 0)
				{
					tsmp->month--;
				}
				_delay_ms(50);
			}
			
			if(btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(16,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
	}
	
	while(1)
	{
		wdt_reset();

		ShowDate(*tsmp, 10, 0);
		lcdGotoXY(16,1);
		lcdPrintData("^^", 2);
		
		btnid = GetPressedButtonId();
		
		if(btnid > 0)
		{
			if (btnid == 2)
			{
				if(tsmp->year < 99)
				{
					tsmp->year++;
				}
				_delay_ms(50);
			}
			
			if(btnid == 3)
			{
				if(tsmp->year > 0)
				{
					tsmp->year--;
				}
				_delay_ms(50);
			}
			
			if(btnid == 4)
			{
				lcdGotoXY(0,1);
				lcdPrintData("                    ", 20);
				lcdGotoXY(16,1);
				lcdPrintData("^^", 2);
				break;
			}
		}
	}
}


int8_t Interpret(char *chstr, uint16_t size)
{
	unsigned char error;
	uint16_t local_idx=0;
	uint16_t local_idx02=0;
	uint16_t dev_id = 0;
	uint16_t f_code = 0;
	uint16_t start_address = 0;
	uint16_t data_count = 0;
	uint16_t crcLocal=0;
	uint16_t crcRemote = 0;
	uint8_t  inData[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		
	uint16_t crcTx = 0;
		
	char chDevId[5]={0,0,0,0,0};
	char chFcode[5]={0,0,0,0,0};
	char chStartAddress[8]={0,0,0,0,0,0,0,0};
	char chDataCount[8]={0,0,0,0,0,0,0,0};
		
	char chDataNos[2]={0,0};
	uint8_t deci,unnit,licenseH=4,licenseL=210;
	char lisen[10]={0,0,0,0,0,0,0,0,0,0};
	uint16_t license=1234;
	
	char chNull[10]={0,0,0,0,0,0,0,0,0,0};
	char chCRC[10]={0,0,0,0,0,0,0,0,0,0};
	char chCRCremote[10]={0,0,0,0,0,0,0,0,0,0};
	char chByte01[5] = {0,0,0,0,0};
	char chCRClocal[10] = {0,0,0,0,0,0,0,0,0,0};
	
	char chBattDigits[10]={0,0,0,0,0,0,0,0,0,0};
	
	TIMESTAMP date_time;
	
	if (chstr[0] == ':')
	{
		if (size >= 17)
		{
			
			for (local_idx=1; local_idx < (size-5); local_idx += 2)
			{
				strncpy(chByte01, &chstr[local_idx], 2);
				inData[local_idx02] = (uint8_t)strtol(chByte01, &chNull, 16);
				local_idx02++;
			}
			
			strncpy(chCRCremote, &chstr[(size-5)], 4);
			crcRemote = (uint16_t)strtol(chCRCremote, &chNull, 16);
			//crcRemote = ((crcRemote >> 8)|(crcRemote << 8));
			crcLocal = CRC16cal(inData, local_idx02);
			crcLocal = ((crcLocal >> 8)|(crcLocal << 8));
			
			sprintf(chCRClocal, "%04X", crcLocal);
/*			
			lcdGotoXY(0, 3);
			lcdPrintData(chCRCremote, 4);
			lcdPrintData("  ", 2);
			lcdPrintData(chCRClocal, 4);
			lcdPrintData(" ", 1);
			
			if (crcLocal == crcRemote)
			{
				lcdPrintData("Y",1);
			}
			else
			{
				lcdPrintData("N", 1);
				//return -2;
			}
*/			
			strncpy(chDevId, (const char*)&chstr[1], 2);
			dev_id = (uint16_t)strtol(chDevId, &chNull, 16);
			
			if (dev_id == 101)
			{
				strncpy(chFcode, (const char*)&chstr[3], 2);
				f_code = (uint16_t)strtol(chFcode, &chNull, 16);
				
				if (f_code == 3)
				{
					strncpy(chStartAddress, (const char*)&chstr[5], 4);
					start_address = (uint16_t)strtol(chStartAddress, &chNull, 16);
					
					if (start_address == 40001)
					{
						//ONLY 16 BIT ENERGY DATA WILL BE TRANSFERED.
						//strncpy(chDataCount, (const char*)&chstr[9], 4);						
						sprintf(chDataCount, "%04X", 1);
						
						//data_count = 1; //(uint16_t)strtol(chDataCount, &chNull, 16);
						strncpy(chDataCount, (const char*)&chstr[9], 4);
						strncpy(chDataNos, (const char*)&chstr[11], 2);
						
						deci=chDataNos[0]-48;
						unnit=chDataNos[1]-48;
						
						chDataNos[1]=(unnit*2)+48;
						
						data_count = (uint16_t)strtol(chDataCount, &chNull, 16);
						
						sprintf(chDataNos, "%02X", (data_count*2));
						
						if (data_count <= 25)
						{
							sbi(PORTD, PD3);
							_delay_ms(100);
							
							ch_txData[0] = 0;
							strncat(ch_txData, ":", 1);
							strncat(ch_txData, chDevId, 2);
							strncat(ch_txData, chFcode, 2);
							//strncat(ch_txData, chStartAddress, 4);
							//strncat(ch_txData, chDataCount, 4);
							//strncat(ch_txData, chDataNos, 2);
							
							USART_TransmitString(ch_txData);
							
							ch_txData[0] = 0;
							strncat(ch_txData, chDataNos, 2);
							
							USART_TransmitString(ch_txData);
							
							txDataNumBuffer[0] = dev_id;
							txDataNumBuffer[1] = f_code;
							txDataNumBuffer[2] = (uint8_t)(start_address >> 8);
							txDataNumBuffer[3] = (uint8_t)(start_address);
							txDataNumBuffer[4] = (uint8_t)(data_count >> 8);
							txDataNumBuffer[5] = (uint8_t)(data_count);
							
							local_idx02 = 6;
							
							for (local_idx=0;local_idx < data_count; local_idx++)
							{
								if(local_idx==0){
									sprintf(chBattDigits, "%04X", (PresentIntigratedUnits >> 16));
									transmitString(chBattDigits);
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedUnits >> 24);
									local_idx02++;
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedUnits >> 16);
									local_idx02++;
								}
								else if(local_idx==1){
									sprintf(chBattDigits, "%04X", PresentIntigratedUnits);
									transmitString(chBattDigits);
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedUnits >> 8);
									local_idx02++;
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedUnits);
									local_idx02++;
								}
								else if(local_idx==2){
									sprintf(chBattDigits, "%04X", PresentIntigratedMiliUnits);
									transmitString(chBattDigits);
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedMiliUnits >> 8);
									local_idx02++;
									txDataNumBuffer[local_idx02] = (uint8_t)(PresentIntigratedMiliUnits);
									local_idx02++;
								}
								else if(local_idx==3){
									sprintf(chBattDigits, "%04X", volt001);
									transmitString(chBattDigits);
									txDataNumBuffer[local_idx02] = (uint8_t)(volt001 >> 8);
									local_idx02++;
									txDataNumBuffer[local_idx02] = (uint8_t)(volt001);
									local_idx02++;
								}
								else if(local_idx==4){
									sprintf(chBattDigits, "%04X", curr001);
									transmitString(chBattDigits);
									txDataNumBuffer[local_idx02] = (uint8_t)(curr001 >> 8);
									local_idx02++;
									txDataNumBuffer[local_idx02] = (uint8_t)(curr001);
									local_idx02++;
								}	
							}
							txDataNumBuffer[local_idx02] = (uint8_t)(licenseH); local_idx02=local_idx02+1;
							txDataNumBuffer[local_idx02] = (uint8_t)(licenseL); local_idx02=local_idx02+1;
							
							crcTx = CRC16cal(txDataNumBuffer, local_idx02);
							crcTx = ((crcTx >> 8)|(crcTx << 8));
							sprintf(chCRC, "%04X", crcTx);
							//GetNDigitFromShort(0x8ACB, chCRC, 4);
							sprintf(lisen, "%04X", license);
							transmitString(lisen);
							transmitString(chCRC);
							transmitString("\r\n");
							
							
							_delay_ms(100);
							cbi(PORTD, PD3);
						}
						
					}
					
				}
				
			}
		}
	}
	else if (chstr[0] == 101)
	{
		if (size == 9)
		{
			date_time.hour = chstr[1];
			date_time.min = chstr[2];
			date_time.sec = chstr[3];
			date_time.day = chstr[4];
			date_time.month = chstr[5];
			date_time.year = chstr[6];
			SetupClock(date_time);
			return 1;
		}
	}
	else if (chstr[0] == 121)
	{
		if (size == 3)
		{
			//findFiles(GET_LIST, 0);
			ReadRtableAndTransmit();
			return 1;
		}
	}
	else if (chstr[0] == 123)
	{
		if (size == 3)
		{
			setting.DataLoggingEnable = YES;
			return 1;
		}
	}
	else if (chstr[0] == 122)
	{
		if (size==3)
		{
			ReadRecordsAndTransmit(15, 1UL);
			return 1;
		}
	}
	else if (chstr[0]==124)
	{
		if (size==3)
		{
			ResetRecordCount();
		}
	}
	else if (chstr[0]==125)
	{
		if (size==3)
		{
			setting.DataLoggingEnable = NO;
		}
	}
	else if (chstr[0]==126)
	{
		if (size==3)
		{
			//memoryStatistics();
		}
	}
	if (chstr[0]==127)
	{
		ReadRecordsAndTransmit(15, 2UL);
	}
	else if (chstr[0]==128)
	{
		ReadRecordsAndTransmit(15, 3UL);
	}
	else if (chstr[0]==132)
	{
		SD_erase(0UL, 10);
	}
	else if (chstr[0]==129)
	{
		ReadRecordsAndTransmitAll();
	}
	else if (chstr[0]==134)
	{
		strcpy_P(buffer, PSTR("data set for writting to the sd card single sector"));
		SD_writeSingleBlock(0UL);
	}
	else if (chstr[0] == 55)
	{
		while (1)
		{
			asm volatile ("nop\n");
		}
	}
	
	return -1;
}

//*************************** MAIN *******************************//
int main(void)
{
	unsigned char option, error, data;
	unsigned int i;
	
	uint16_t dispValue = 0;
	
	char chnum01[7];
	char chnum02[7];
	char chVolt001[7];
	char chCurr001[7];
	
	char chEnergy[9];
	char chwatt[9];
	
	char chSecCounter[8];
	
	uint8_t lcdPageNo = 1;
	uint8_t btnid=0;
	
	uint8_t baud = 103;
	
	uint8_t blinkToggler = 1;
	
	uint8_t localidx001=0;

	_delay_ms(100);  //delay for VCC stabilization

	init_devices();
	
	MCP_GPIO_Init();
	
	USART_Init(baud);

	cardType = 0;

	lcdInit();
	delay_ms(10);
	////<MOD>CheckFaultSetting();
	////<MOD>GetLoopedSettings(&setting);
	SetupAdc(ADC_MODE_NOMRAL_FREE);
	StartAdc();
	lcdGotoXY(0,0);
	lcdPrintData("DIGITIZE 0x00 ......", 20);
		
	INIT_IO;
	
	TIMESTAMP date_time;
		
	RestoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
	
	if (calibvalues.calib_volt_001 >= 0XAAAA)
	{
		calibvalues.calib_volt_001 = 472;
		calibvalues.calib_curr_001 = 818;
		calibvalues.curr_adc_offset001 = 2033;//2044
		StoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
	}
	
	PresentIntigratedUnits = GetLastUnit();

	if(PresentIntigratedUnits > 0xAAAAFFFF)
	{
		PresentIntigratedUnits = 0;
	}
	
	PresentIntigratedMiliUnits = eeprom_read_word((const uint16_t*)40);
	
	if (PresentIntigratedMiliUnits > 0xAAAA)
	{
		PresentIntigratedMiliUnits = 0;
	}
		
	sei();
	
	wdt_enable(WDTO_2S);
	
	lcdClear();
	
	//USART_Init(baud);
	
	while (1)
	{
		/*
		GetNDigitFromShort(calibvalues.calib_volt_001, chnum01, 4);
		GetNDigitFromShort(calibvalues.calib_curr_001, chnum02, 4);
		lcdGotoXY(0,0);
		lcdPrintData("V-FAC:", 6);
		lcdPrintData(chnum01, 4);
		lcdGotoXY(0,1);
		lcdPrintData("I-FAC:", 6);
		lcdPrintData(chnum02, 4);
		*/
		_delay_ms(20);
		wdt_reset();
		
		localidx001++;
		
		if (localidx001 > 2)
		{
			localidx001 = 0;
			break;
		}
	}
	
	/* <RTC>
	date_time.hour = 14;
	date_time.min = 12;
	date_time.sec = 10;
	date_time.day = 23;
	date_time.month = 6;
	date_time.year = 17;
	SetupClock(date_time);
	*/
	
	lcdClear();

	while(1)
	{
		cbi(PORTD, PD3);
		if (SecondTickInt == 1)
		{
			
			SecondTickInt = 0;
			
			//GetNDigitFromShort(adc_main, chadc01, 4);
			
			
			if (PrimarySecCount < 10)
			{
				PrimarySecCount++;
			}
			else
			{
				PrimarySecCount=0;
				
			}
			
			mcp_adc_channel01 = MCP_Read(1);
			mcp_adc_channel01 = MCP_Read(1);
			
			//lcdGotoXY(0,1);
			//lcdPrintData("Energy::", 8);
			
			if (mcp_adc01_acq_count < 10)
			{
				mcp_adc01_sum += mcp_adc_channel01;
				mcp_adc01_acq_count++;
				//volt001 = (mcp_adc_channel01 * 100l) / calibvalues.calib_volt_001; //(adc_channel1 * 100) / calibvalues.calib_volt_001;
			}
			else
			{
				mcp_adc_channel01 = mcp_adc01_sum / 10;
				volt001 = (mcp_adc_channel01 * 100l) / calibvalues.calib_volt_001; //(adc_channel1 * 100) / calibvalues.calib_volt_001;
				mcp_adc01_acq_count = 0;
				mcp_adc01_sum = 0;
			}
			
			mcp_adc_temp = MCP_Read(0);
			_delay_ms(20);
			
			//mcp_adc_channel02 = MCP_Read(0);
			
			if (mcp_adc02_acq_count < 10)
			{
				//mcp_adc_temp = MCP_Read(0);
				mcp_adc_temp = MCP_Read(0);
				mcp_adc02_sum += mcp_adc_temp;
				
				mcp_adc02_acq_count++;
			}
			else
			{
				mcp_temp2 = mcp_adc02_sum / 10l;
				//mcp_adc_channel02 = mcp_adc_temp;
				mcp_adc02_acq_count = 0;
				mcp_adc02_sum = 0l;
			}
			
			mcp_adc_channel02 = mcp_temp2;
			mcp_adc_channel03 = mcp_adc_channel02;
			
			if (mcp_adc_channel02 > 0)
			{
				
				if (mcp_adc_channel02 < calibvalues.curr_adc_offset001)
				{
					curr001 = 0;
				}
				else
				{
					mcp_adc_channel02 -= calibvalues.curr_adc_offset001;
					curr001 = (mcp_adc_channel02 *1000l) / calibvalues.calib_curr_001;
				}
				
				//curr001 = (adc_channel2 *100) / calibvalues.calib_curr_001;
			}
			else
			{
				curr001 = 0;
			}
			
			lcdGotoXY(0,0);
			lcdPrintData("  DC ENERGY METER   ", 20);
			
			//curr001 = adc_channel2;
			//volt001 = adc_channel1;
			lcdGotoXY(0,2);
			GetNDigitFromShort(volt001, chVolt001, 3);
			lcdPrintData("VOLT :",6);
			lcdPrintData(chVolt001, 2);
			lcdPrintData(".", 1);
			lcdPrintData(&chVolt001[2], 1);
			lcdPrintData("          ",10);
			
			lcdGotoXY(0, 3);
			lcdPrintData("CURR :",6);
			GetNDigitFromShort(curr001, chCurr001, 3);
			lcdPrintData(chCurr001, 2);
			lcdPrintData(".", 1);
			lcdPrintData(&chCurr001[2], 1);
			lcdPrintData("          ",10);
			
			//lcdGotoXY(11, 3);
			//lcdPrintData("AD:",3);
			//GetNDigitFromShort(mcp_adc_channel03, chnum02, 4);
			//lcdPrintData(chnum02, 4);
			
			lcdGotoXY(0,1);
			lcdPrintData("KWH  :", 6);
			//GetNDigitFromShort(PresentIntigratedUnits, chEnergy, 5);
			//sprintf(chEnergy, "%05d", PresentIntigratedUnits);
			GetNDigitFromu32(PresentIntigratedUnits, chEnergy, 5);
			lcdPrintData(chEnergy, 5);
			GetNDigitFromShort(PresentIntigratedMiliUnits, chEnergy, 2);
			lcdPrintData(".", 1);
			lcdPrintData(chEnergy, 2);
			lcdPrintData("    ", 4);
			
			////lcdGotoXY(10, 2);
			////lcdPrintData("W:", 2);
			////GetNDigitFromShort(wattPerSec, chwatt, 5);
			////lcdPrintData(chwatt, 5);

			/*<RTC>
			ReadRtcI2cTime(&tstamp);
			ShowDateTime(tstamp, 0, 3);
			*/
			
			/*
			GetNDigitFromShort(sec_counter, chSecCounter, 4);
			lcdGotoXY(11, 3);
			lcdPrintData(chSecCounter, 4);
			*/
		}
		
		if (T2TickInt002 == 1)
		{
			T2TickInt002 = 0;
		}
		
		if (PresentIntigratedMiliUnits >= 100)
		{
			PresentIntigratedUnits++;
			SetLastUnit(PresentIntigratedUnits);
			PresentIntigratedMiliUnits = 0;
		}
		
		UpdateEnergyCalculation();
		
		btnid = GetPressedButtonId();

		if (btnid > 0)
		{
			if (btnid == 1)
			{
				//SetAndShowU16(&calibvalues.curr_adc_offset001, "OFSET:");
				//StoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
				_delay_ms(200);
			}
			
			if (btnid == 2)
			{
				SetAndShowU16(&calibvalues.calib_curr_001, "I-FAC:");
				StoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
				_delay_ms(200);
				
				SetAndShowU16(&calibvalues.calib_volt_001, "V-FAC:");
				StoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
				_delay_ms(200);
				
				SetAndShowU16(&calibvalues.curr_adc_offset001, "OFSET:");
				StoreBuffer((unsigned char*)&calibvalues, sizeof(CALIBVALUES));
				_delay_ms(200);
			}
			
			if (btnid == 3)
			{
				sec_counter = 0;
				_delay_ms(50);
			}
			
			if (btnid == 4)
			{
				//SetupTimeStamp("SET TIME: ", &tstamp);
				//SetupDateStamp("SET DATE:", &tstamp);
				//SetupClock(tstamp);
				_delay_ms(50);
			}
			
			
		}
		
		if (rxComplete == 1)
		{
			rxSize = ReadRxRawBuffer(chuart01);
			if (rxSize > 0)
			{
				chuart01[rxSize] = 0;
				
				//USART_TransmitString("DATA\r\n");
				//USART_TransmitString(chuart01);
				//USART_TransmitString("\r\n");
				
				if (Interpret(chuart01, rxSize) == 1)
				{
					;//USART_TransmitString("OK\r\n");
				}
				else
				{
					;//USART_TransmitString("ERROR\r\n");
				}
			}
		}
		
		wdt_reset();
		
	}
	return 0;
}
//********** END *********** www.dharmanitech.com *************
