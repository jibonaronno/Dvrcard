

#ifndef LCDX_H
#define LCDX_H
		
#define LCD_CTRL_PORT	PORTB
#define LCD_CTRL_DDR	DDRB
#define LCD_CTRL_RS		PB0
#define LCD_CTRL_RW		8
#define LCD_CTRL_E		PB1

#define LCD_DATA_POUT	PORTD
#define LCD_DATA_PIN	PIND
#define LCD_DATA_DDR	DDRD

#define LCD_CTRL_ADDR	0x1000

#define LCD_DATA_ADDR	0x1001

#define LCD_LINES				4	// visible lines
#define LCD_LINE_LENGTH			20	// line length (in characters)
// cursor position to DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54

#define LCD_DATA_4BIT

#ifndef LCD_DELAY
#if F_CPU >= 16000000
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
#else
#if FCPU >= 12000000
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
#else
#if FCPU >= 8000000
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
#else
#if FCPU >= 4000000
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n");
#else
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n");
#endif
#endif
#endif
#endif
#endif

#define LCD_CLR             0      // DB0: clear display
#define LCD_HOME            1      // DB1: return to home position
#define LCD_ENTRY_MODE      2      // DB2: set entry mode
#define LCD_ENTRY_INC       1      //   DB1: increment
#define LCD_ENTRY_SHIFT     0      //   DB2: shift
#define LCD_ON_CTRL         3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2      //   DB2: turn display on
#define LCD_ON_CURSOR       1      //   DB1: turn cursor on
#define LCD_ON_BLINK        0      //   DB0: blinking cursor
#define LCD_MOVE            4      // DB4: move cursor/display
#define LCD_MOVE_DISP       3      //   DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      //   DB2: move right (0-> left)
#define LCD_FUNCTION        5      // DB5: function set
#define LCD_FUNCTION_4BIT   0      //   DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_8BIT   4      //   DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      //   DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2      //   DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM           6      // DB6: set CG RAM address
#define LCD_DDRAM           7      // DB7: set DD RAM address
// reading:
#define LCD_BUSY            7      // DB7: LCD is busy

#ifdef LCD_DATA_4BIT
	#define LCD_FDEF_1			(0<<LCD_FUNCTION_4BIT)
#else
	#define LCD_FDEF_1			(1<<LCD_FUNCTION_8BIT)
#endif

#define LCD_FDEF_2				(1<<LCD_FUNCTION_2LINES)

//<CDTR>
#define LCD_FUNCTION_DEFAULT	((1<<LCD_FUNCTION) | LCD_FDEF_1 | LCD_FDEF_2)

#define LCD_MODE_DEFAULT		((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC))


// initializes I/O pins connected to LCD
void lcdInitHW(void);
// waits until LCD is not busy
void lcdBusyWait(void);
// writes a control command to the LCD
void lcdControlWrite(uint8_t data);
// read the control status from the LCD
uint8_t lcdControlRead(void);
// writes a data byte to the LCD screen at the current position
void lcdDataWrite(uint8_t data);
// reads the data byte on the LCD screen at the current position
uint8_t lcdDataRead(void);


// ****** High-levlel functions ******
// these functions provide the high-level control of the LCD
// such as clearing the display, setting cursor positions,
// displaying text and special characters

// initializes the LCD display (gets it ready for use)
void lcdInit(void);

// moves the cursor/position to Home (upper left corner)
void lcdHome(void);

// clears the LCD display
void lcdClear(void);

// moves the cursor/position to the row,col requested
// ** this may not be accurate for all displays
void lcdGotoXY(uint8_t row, uint8_t col);

// loads a special user-defined character into the LCD
// <lcdCustomCharArray> is a pointer to a ROM array containing custom characters
// <romCharNum> is the index of the character to load from lcdCustomCharArray
// <lcdCharNum> is the RAM location in the LCD (legal value: 0-7)
//void lcdLoadCustomChar(u08* lcdCustomCharArray, u08 romCharNum, u08 lcdCharNum);

// prints a series of bytes/characters to the display
void lcdPrintData(char* data, uint8_t nBytes);

#endif
