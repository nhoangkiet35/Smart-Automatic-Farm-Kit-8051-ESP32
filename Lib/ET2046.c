#include "ET2046.h"
#include "delay.h"

/**
 * Kh?i t?o các chân SPI
 */
void ET2046_Init(void)
{
    CS = 1;
    CLK = 0;
}

/**
 * G?i 1 byte l?nh ð?n ET2046 qua SPI m?m
 */
void ET2046_Write(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DIN = (dat & 0x80) ? 1 : 0; // Bit cao nh?t trý?c
        CLK = 1;
        delay(1);
        CLK = 0;
        delay(1);
        dat <<= 1;
    }
}

/**
 * Ð?c giá tr? 12-bit t? kênh X c?a ET2046
 * (Thý?ng dùng X+ n?i v?i tín hi?u analog)
 */
unsigned int ET2046_ReadX(void)
{
    unsigned int value = 0;
    unsigned char i;

    CS = 0;             // Ch?n ADC
    ET2046_Write(0x90); // Command ch?n kênh X (1001 0000)

    delay(1); // Ch? chuy?n ð?i

    for (i = 0; i < 12; i++) // Ð?c 12 bit ADC
    {
        value <<= 1;
        CLK = 1;
        if (DOUT)
            value |= 0x01;
        CLK = 0;
        delay(1);
    }

    CS = 1; // B? ch?n ADC

    return value; // Tr? v? giá tr? 0–4095
}
