/*
 * ButtonMatrix.h
 *
 * Created: 7/24/2013 8:38:19 AM
 *  Author: aronno
 */ 
#include <stdint.h>

#ifndef BUTTONMATRIX_H_
#define BUTTONMATRIX_H_

/************************************************************************/
/*	4X4 Button Matrix Operation. The code is designed to be reusable.	*/
/*	When migrating from one processor to another, Reconfigure the defines
	should be enough to migrate. Migration to another processor like
	ARM may need extra integration at function level modification. Here
	we are trying to limit this features at definition level.	*/
/************************************************************************/

#define		INPORT		PORTA
#define		INPIN		PINA
#define		OUTPORT		PORTA
#define		INDDR		DDRA
#define		OUTDDR		DDRA


#define		INPIN1		PA4
#define		INPIN2		PA5
#define		INPIN3		PA6
#define		INPIN4		PA7

#define		OUTPIN1		PA4
#define		OUTPIN2		PA5
#define		OUTPIN3		PA6
#define		OUTPIN4		PA7


#define		SETINDDR1	cbi(INDDR, INPIN1)
#define		SETPULLUP1	sbi(INPORT, INPIN1)

#define		SETINDDR2	cbi(INDDR, INPIN2)
#define		SETPULLUP2	sbi(INPORT, INPIN2)

#define		SETINDDR3	cbi(INDDR, INPIN3)
#define		SETPULLUP3	sbi(INPORT, INPIN3)

#define		SETINDDR4	cbi(INDDR, INPIN4)
#define		SETPULLUP4	sbi(INPORT, INPIN4)

#define		CHECKPIN1	if(!(INPIN & (1 << INPIN1)))
#define		CHECKPIN2	if(!(INPIN & (1 << INPIN2)))
#define		CHECKPIN3	if(!(INPIN & (1 << INPIN3)))
#define		CHECKPIN4	if(!(INPIN & (1 << INPIN4)))

/************************************************************************/
/*	Because the LSB 4 bits are always 1111 for being pulled up at the out 
	port. */
/************************************************************************/
#define		ACTIVE_COLUMN1		OUTPORT = 0b11101111
#define		ACTIVE_COLUMN2		OUTPORT = 0b11011111
#define		ACTIVE_COLUMN3		OUTPORT = 0b10111111
#define		ACTIVE_COLUMN4		OUTPORT = 0b01111111

#define		BTNMATRIX_COLUMN1		1
#define		BTNMATRIX_COLUMN2		2
#define		BTNMATRIX_COLUMN3		3
#define		BTNMATRIX_COLUMN4		4

//#define		SHORTPRESS_COUNT		100
//#define		LONGPRESS_COUNT			1000

uint8_t GetPressedButtonId();

uint8_t GetLongPressedButtonId();

void ButtonMatrixPrimaryCycle();

//<MOD> uint16_t KBInput(uint8_t lcdx, uint8_t lcdy, char *chkb);

#endif /* BUTTONMATRIX_H_ */