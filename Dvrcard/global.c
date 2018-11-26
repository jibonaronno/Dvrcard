/*
 * global.c
 *
 * Created: 8/22/2013 9:27:43 PM
 *  Author: Shahidullah
 */ 

#include <stdint.h>
#include "global.h"

void __delay_cycle(uint32_t count)
{
	uint32_t i=0;
	for (i=0;i<count;i++)
	{
		asm volatile ("nop");
	}
}

void delay_us(unsigned short time_us)
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty)

	// one loop takes 5 cpu cycles
	for (i=0; i < delay_loops; i++) {};
}

void delay_ms(unsigned char time_ms)
{
	unsigned short delay_count = F_CPU / 4000;

	unsigned short cnt;
	
	//#ifndef  _DEBUG
	asm volatile ("\n"
	"L_dl1%=:\n\t"
	"mov %A0, %A2\n\t"
	"mov %B0, %B2\n"
	"L_dl2%=:\n\t"
	"sbiw %A0, 1\n\t"
	"brne L_dl2%=\n\t"
	"dec %1\n\t" "brne L_dl1%=\n\t":"=&w" (cnt)
	:"r"(time_ms), "r"((unsigned short) (delay_count))
	//#endif // _DEBUG
	
	);
}
