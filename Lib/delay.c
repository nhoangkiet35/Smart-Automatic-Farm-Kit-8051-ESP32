/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#include "delay.h"

/**
 * @brief Creates blocking software delay
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
 *		Time value = 65536 - ((Delay time (�s) * Crystal Frequency (Hz)) / Timer Clock Division)
 *
 * Where: TCD usually 12 in standard 8051
 * Time value = 65536 - (1000 * 0.9216) = 64614
 * => TH0 = 64614/256 = 252 (0xFC), TL0 = 64614%256 = 66 (0x42)
 */