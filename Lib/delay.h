/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 *
 * Purpose: Provides a blocking delay function for the 8051 microcontroller, calibrated for an 11.0592 MHz crystal.
 */
#ifndef DELAY_H
#define DELAY_H

#include <REGX52.H>

volatile unsigned long millis_counter = 0;

/**
 * @brief Delay with 11.0592MHz crystal
 */
void delay(unsigned int count);

/**
 * @brief Use Timer0 in mode 1 (16-bit)
 * Configure the Timer to generate an interrupt every 1ms
 */
void Timer0_Init(void);
unsigned long Timer0_GetMillis(void);

#endif /* DELAY_H */