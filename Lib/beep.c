/*
 * Created Date: Wednesday, July 16th 2025, 3:08:05 pm
 * Author: Hoang Kiet
 */
#include "beep.h"

/**
 * @brief Create delay using Timer0
 * @param time: Delay time in µs (microseconds)
 * Limit: time < 65536 (because Timer0 is 16-bit)
 */
void delay_beep(unsigned int time)
{
    TMOD &= 0xF0; // Clear Timer0 configuration (lowest 4 bits)
    TMOD |= 0x01; // Timer0 Mode 1 (16-bit)

    // Load counter value corresponding to time
    TH0 = (65536 - time) >> 8;
    TL0 = (65536 - time) & 0xFF;

    TF0 = 0; // Clear the overflow flag

    TR0 = 1; // Start counting

    while (TF0 == 0)
        ; // Wait until Timer overflows

    TR0 = 0; // Turn off Timer
    TF0 = 0; // Clear the overflow flag
}

/**
 * @brief Simple delay using loop (not exact)
 */
void delay_beep2(unsigned int time)
{
    while (time--)
        ;
}

/**
 * @brief Play a beep with the specified number of times and period
 * @param period: Beep period (in ms) — will divide by 2 to turn on/off
 * @param t: number of beeps
 */
void beep(unsigned int period, unsigned int t)
{
    unsigned int i;
    unsigned int delay_time_us = (period * 1000) / 2; // Convert ms to us

    for (i = 0; i < t; i++)
    {
        buzzer = 1;
        delay_beep(delay_time_us); // Turn on the buzzer half-cycle
        buzzer = 0;
        delay_beep(delay_time_us); // Turn off the buzzer half-cycle
    }
}