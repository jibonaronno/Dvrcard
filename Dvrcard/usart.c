/*
 * usart.c
 *
 * Created: 7/25/2013 2:25:02 AM
 *  Author: acer
 */ 

#include "usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

char rxrawbuffer[20];
uint8_t rxrawbufferidx = 0;
uint8_t rxBufferValid = 0;

/************************************************************************/
/*	This flag indicates when sequence of serial data stops arriving.
	A Cycle Function can help to detect the event. An watchdog counter
	can be used to indicate the repeat of data arriving. */
/************************************************************************/
volatile uint8_t rxComplete = 0;  
uint16_t rxWatchDogCounter = 0;

ISR(USART_TXC_vect)
{
	//cbi(PORTD, PD2);
}

/************************************************************************/
/*	When reading rx, If rx buffer count is larger than 20 then reset the 
	buffer                                                              */
/************************************************************************/
ISR(USART_RXC_vect)
{
	
	if (rxrawbufferidx > 19)
	{
		rxrawbufferidx = 0;
	}
	
	rxrawbuffer[rxrawbufferidx] = USART_Receive();
	rxrawbufferidx++;
	rxBufferValid = 1;
	
}

/************************************************************************/
/*	Read the raw rx buffer into the parameter *buf and return the length 
	of the buffer.                         */
/************************************************************************/
uint8_t ReadRxRawBuffer(char *buf)
{
	uint8_t localidx = 0;
	if (rxrawbufferidx > 0)
	{
		for (localidx=0;localidx<rxrawbufferidx;localidx++)
		{
			buf[localidx] = rxrawbuffer[localidx];
		}
		
		localidx = rxrawbufferidx; //use localidx variable for temporary use to return the value;
		rxrawbufferidx=0;
		rxBufferValid = 0;
		return localidx;
	}
	return 0;
}

void ClearRxRawBuffer(void)
{
	rxrawbufferidx=0;
	rxBufferValid = 0;
}

void RxWatchdogCountUpCycle()
{
	if (rxBufferValid == 1)
	{
		if (rxWatchDogCounter < 50)
		{
			rxWatchDogCounter++;
		}
		else
		{
			rxComplete = 1;
		}
	}
	else
	{
		rxWatchDogCounter = 0;
		rxComplete = 0;
	}
}

void USART_Init(unsigned int baud)
{
	UBRRH = (unsigned char)(baud>>8);

	UBRRL = (unsigned char)baud;

	//UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	
	UCSRB = (1<<TXEN)|(1<<RXEN)|(1<<RXCIE); //|(1<<TXCIE)

	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

char USART_Receive(void)
{
	while(!(UCSRA & (1<<RXC)))
	;
	return UDR;
}

void USART_transmit(unsigned char data)
{
	while(!(UCSRA & (1<<UDRE)))
	;

	UDR = data;
}

void USART_TransmitString(char *dat)
{
	unsigned char size = strlen((const char*)dat);

	unsigned char i = 0;

	for(i=0;i<size;i++)
	{
		USART_transmit(dat[i]);
	}
}
