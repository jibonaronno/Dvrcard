/*
 * lcdx.c
 *
 * Created: 8/22/2013 8:43:24 PM
 *  Author: Shahidullah
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "lcdx.h"

void lcdInitHW(void)
{
	// initialize LCD control lines
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);
	// initialize LCD control lines to output

	delay_ms(1);

	sbi(LCD_CTRL_DDR, LCD_CTRL_RS);
	sbi(LCD_CTRL_DDR, LCD_CTRL_RW);
	sbi(LCD_CTRL_DDR, LCD_CTRL_E);
	delay_ms(1);
}

void lcdControlWrite(uint8_t data)
{

	delay_ms(2);
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);			// set RS to "control"
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);			// set R/W to "write"

	sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line

	outb(LCD_DATA_DDR, ((inb(LCD_DATA_DDR)&0b00001111)|0b11110000));	// set data I/O lines to output (4bit)

	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | ((data)&0xF0) );

	cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line

	sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line

	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | ((data<<4)&0xF0) );

	cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
}

void lcdDataWrite(uint8_t data)
{

	delay_ms(1);
	LCD_DELAY;								// wait
	LCD_DELAY;								// wait
	sbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "data"
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "write"
	sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
	LCD_DELAY;								// wait
	//LCD_DELAY;								// wait
	outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0b11110000);	// set data I/O lines to output (4bit)
	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | ((data)&0xF0) );	// output data, high 4 bits
	LCD_DELAY;								// wait
	//LCD_DELAY;								// wait
	//delay_ms(1);
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	//delay_ms(1);
	//LCD_DELAY;								// wait
	LCD_DELAY;								// wait
	sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
	LCD_DELAY;								// wait
	//LCD_DELAY;								// wait
	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | ((data<<4)&0xF0) );	// output data, low 4 bits
	LCD_DELAY;								// wait
	//LCD_DELAY;								// wait
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	//LCD_DELAY;								// wait
	LCD_DELAY;								// wait
	//delay_ms(1);
}

void lcdInit()
{
	delay_ms(1);
	lcdInitHW();
	lcdControlWrite(0x00);
	delay_ms(1);
	lcdControlWrite(LCD_FUNCTION_DEFAULT);
	lcdControlWrite(1<<LCD_CLR);
	delay_ms(100);
	delay_ms(1);
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	delay_ms(1);
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	delay_ms(1);
	delay_ms(1);
	delay_ms(1);
}

void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}

void lcdClear(void)
{
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
}

void lcdGotoXY(uint8_t x, uint8_t y)
{
	register uint8_t DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
		case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
		case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
		case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
		case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
		default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
}

void lcdPrintData(char* data, uint8_t nBytes)
{
	register uint8_t i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		lcdDataWrite(data[i]);
	}
}

