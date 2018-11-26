/*
 * MCP2303soft.h
 *
 * Created: 12/7/2016 5:03:21 PM
 *  Author: ASUS
 */ 


#ifndef MCP2303SOFT_H_
#define MCP2303SOFT_H_

#define MCP_SPI_PORT		PORTB
#define MCP_SPI_DDR			DDRB

//BELOW MOSI PIN PB5
#define MCP_DIN_PIN			PB5
#define MCP_DOUT_PIN		PB6
#define MCP_SS_PIN1			PB4
#define MCP_SCK_PIN			PB7

void MCP_GPIO_Init(void);

uint16_t MCP_Read(uint8_t chno);

#endif /* MCP2303SOFT_H_ */