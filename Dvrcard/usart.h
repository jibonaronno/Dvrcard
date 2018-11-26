#include <stdint.h>

void USART_Init(unsigned int baud);

char USART_Receive(void);

void USART_transmit(unsigned char data);

void USART_TransmitString(char *dat);

void RxWatchdogCountUpCycle();

uint8_t ReadRxRawBuffer(char *buf);

void ClearRxRawBuffer(void);

