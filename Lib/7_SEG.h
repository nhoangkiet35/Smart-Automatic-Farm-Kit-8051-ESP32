/*
 * Created Date: Wednesday, July 16th 2025, 3:08:31 pm
 * Author: Hoang Kiet
 */
#ifndef _7_SEG_H_
#define _7_SEG_H_

#include <REGX52.H> // Include this if you're using 8051 MCU

// Define segment selector pins
sbit LSA = P2 ^ 2;
sbit LSB = P2 ^ 3;
sbit LSC = P2 ^ 4;

// Function declarations
void show_7seg(unsigned int b);
int decode_7seg(unsigned int a);

#endif // _7_SEG_H_
