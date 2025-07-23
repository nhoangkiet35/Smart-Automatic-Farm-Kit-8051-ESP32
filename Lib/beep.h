/*
 * Created Date: Wednesday, July 16th 2025, 3:08:05 pm
 * Author: Hoang Kiet
 */
#ifndef BEEP_H
#define BEEP_H

#include <REGX52.H>

/**
 * @brief Define buzzer pin (assuming connected to P2.5, adjust if needed)
 */
sbit buzzer = P2 ^ 5;

/**
 * @brief Delay using Timer0 for beep (µs)
 * @param time delay time in µs
 */
void delay_beep(unsigned int time);

/**
 * @brief Simple Delay (not exact, loop form)
 * @param time delay time
 */
void delay_beep2(unsigned int time);

/**
 * @brief Play beep
 * @param period Beep period (µs)
 * @param t Play time (ms)
 */
void beep(unsigned int period, unsigned int t);

#endif
