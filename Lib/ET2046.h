#ifndef ET2046_H
#define ET2046_H

#include <REGX52.H>

/**
 * Ð?nh ngh?a chân SPI n?i ET2046 v?i 8051
 */
sbit DOUT = P3 ^ 4; // MISO - Data from ADC to MCU
sbit DIN = P3 ^ 5;  // MOSI - Data from MCU to ADC
sbit CLK = P3 ^ 6;  // Clock
sbit CS = P3 ^ 7;   // Chip Select

void ET2046_Init(void);
void ET2046_Write(unsigned char dat);
unsigned int ET2046_ReadX(void);

#endif
