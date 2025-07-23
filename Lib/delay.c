/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#include "delay.h"

/**
 * @brief Creates blocking software delay
 * @details Generates approximate delay time by executing NOP operations.
 *          Timing is calibrated for 11.0592MHz crystal oscillator.
 *
 * @param count Number of delay units (approximately milliseconds at 11.0592MHz)
 * @return None
 *
 * @note Actual delay time depends on:
 *       - CPU clock frequency
 *       - Compiler optimization settings
 *       - Interrupt activity
 * @warning This is a busy-wait delay that blocks CPU execution
 */
void delay(unsigned int count)
{
    unsigned int i;
    while (count != 0)
    {
        i = 115; // Tuned value for ~1ms delay at 11.0592MHz
        while (i > 0)
            i--;
        count--;
    }
}

/**
 * @brief Initialize Timer0 to generate interrupt every 1ms
 * @note Formula for calculating Timer0 time value:
 *		Time value = 65536 - ((Delay time (µs) * Crystal Frequency (Hz)) / Timer Clock Division)
 *
 * Where: TCD usually 12 in standard 8051
 * Time value = 65536 - (1000 * 0.9216) = 64614
 * => TH0 = 64614/256 = 252 (0xFC), TL0 = 64614%256 = 66 (0x42)
 */
void Timer0_Init(void)
{
    TMOD &= 0xF0; // Remove configure Timer0
    TMOD |= 0x01; // Mode 1: 16-bit timer
    TH0 = 0xFC;   // Load value to generate ~1ms (11.0592MHz)
    TL0 = 0x42;   // TH0:TL0 = FC66H => 1ms
    ET0 = 1;      // Allow interrupt Timer0
    EA = 1;       // Allow all interrupt
    TR0 = 1;      // Timer0 Run
}

/**
 * @brief Returns number of ms since Timer0 started
 */
unsigned long Timer0_GetMillis(void)
{
    unsigned long ms;
    EA = 0; // Temporarily disable interrupts to avoid errors when reading variables
    ms = millis_counter;
    EA = 1; // Enable interrupt again
    return ms;
}
/**
 * @brief Timer0 interrupt function (triggered every 1ms)
 */
void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC; // Reload to keep cycle 1ms
    TL0 = 0x42;
    millis_counter++;
}