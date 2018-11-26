//#include "support.h"

#define CHANNEL_0		0b00000000
#define CHANNEL_1		0b00000001
#define CHANNEL_2		0b00000010
#define CHANNEL_3		0b00000011
#define CHANNEL_4		0b00000100
#define CHANNEL_5		0b00000101
#define CHANNEL_6		0b00000110
#define CHANNEL_7		0b00000111

#define ADC_MODE_NOMRAL_FREE	101

void SetupAdc(unsigned char mode);
void StartAdc();
void StopAdc();
