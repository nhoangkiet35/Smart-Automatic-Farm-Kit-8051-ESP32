#include "ET2046.h"
#include "delay.h"

/**
 * Kh?i t?o c�c ch�n SPI
 */
void ET2046_Init(void)
{
    CS = 1;
    CLK = 0;
}

/**
 * G?i 1 byte l?nh �?n ET2046 qua SPI m?m
 */
void ET2046_Write(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DIN = (dat & 0x80) ? 1 : 0; // Bit cao nh?t tr�?c
        CLK = 1;
        delay(1);
        CLK = 0;
        delay(1);
        dat <<= 1;
    }
}

/**
 * �?c gi� tr? 12-bit t? k�nh X c?a ET2046
 * (Th�?ng d�ng X+ n?i v?i t�n hi?u analog)
 */
unsigned int ET2046_ReadX(void)
{
    unsigned int value = 0;
    unsigned char i;

    CS = 0;             // Ch?n ADC
    ET2046_Write(0x90); // Command ch?n k�nh X (1001 0000)

    delay(1); // Ch? chuy?n �?i

    for (i = 0; i < 12; i++) // �?c 12 bit ADC
    {
        value <<= 1;
        CLK = 1;
        if (DOUT)
            value |= 0x01;
        CLK = 0;
        delay(1);
    }

    CS = 1; // B? ch?n ADC

    return value; // Tr? v? gi� tr? 0�4095
}
