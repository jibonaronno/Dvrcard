/*
 * MCP2303soft.c
 *
 * Created: 12/7/2016 4:33:29 PM
 *  Author: K.M.M.Haque
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "global.h"
#include "MCP2303soft.h"

void MCP_GPIO_Init(void)
{
	sbi(MCP_SPI_DDR, MCP_SS_PIN1);
	sbi(MCP_SPI_DDR, MCP_SCK_PIN);
	cbi(MCP_SPI_DDR, MCP_DOUT_PIN);
	sbi(MCP_SPI_DDR, MCP_DIN_PIN);
	
	sbi(MCP_SPI_PORT, MCP_SS_PIN1);
	cbi(MCP_SPI_PORT, MCP_SCK_PIN);
	cbi(MCP_SPI_PORT, MCP_DIN_PIN);
}

uint16_t MCP_Read(uint8_t chno)
{
	uint8_t _clk_count;
	
	uint16_t _data=0;
	
	cbi(MCP_SPI_PORT, MCP_SS_PIN1);
	
	__delay_cycle(5);
	
	sbi(MCP_SPI_PORT, MCP_DIN_PIN);
	
	__delay_cycle(5);
	
	sbi(MCP_SPI_PORT, MCP_SCK_PIN);
	
	__delay_cycle(5);
	
	cbi(MCP_SPI_PORT, MCP_SCK_PIN);
	
	__delay_cycle(5);
	
	
	sbi(MCP_SPI_PORT, MCP_DIN_PIN);
	__delay_cycle(5);
	sbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	cbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	
	if (chno == 0)
	{
		cbi(MCP_SPI_PORT, MCP_DIN_PIN);
	}
	else
	{
		sbi(MCP_SPI_PORT, MCP_DIN_PIN);
	}
	__delay_cycle(5);
	sbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	cbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	
	sbi(MCP_SPI_PORT, MCP_DIN_PIN);
	__delay_cycle(5);
	sbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	cbi(MCP_SPI_PORT, MCP_SCK_PIN);
	__delay_cycle(5);
	
	
		sbi(MCP_SPI_PORT, MCP_SCK_PIN);
		__delay_cycle(5);
		cbi(MCP_SPI_PORT, MCP_SCK_PIN);
		__delay_cycle(5);
		
	
	for (_clk_count=0 ;_clk_count < 12; _clk_count++)
	{
		sbi(MCP_SPI_PORT, MCP_SCK_PIN);
		__delay_cycle(5);
		
		if (PINB & (BV(MCP_DOUT_PIN)))
		{
			_data |= 1;
		}
		
		if (_clk_count < 11)
		{
			_data <<= 1;
		}
		
		cbi(MCP_SPI_PORT, MCP_SCK_PIN);
		__delay_cycle(5);
	}
	
	sbi(MCP_SPI_PORT, MCP_SS_PIN1);
	
	return _data;
	
}

