/*
 * Filename: DHT11.c
 * Created Date: Saturday, July 5th 2025, 9:27:29 pm
 * Author: Hoang Kiet
 */
#include "DHT11.h"
#include <stdio.h>

// Define global variables
int I_RH, D_RH, I_Temp, D_Temp, CheckSum;

void timer_delay20ms(void)
{
    /*
     * Function: Create a delay of ~20ms using Timer 0 (mode 1, 16-bit).
     * Application: Used to create a low pulse of 20ms when sending a request signal to DHT11.
     * Pulse: A low signal lasting 20ms to activate DHT11.
     */
    TMOD &= 0xF0;
    TMOD |= 0x01; // Timer 0, mode 1 (16-bit)
    TH0 = 0xB8;   // Load value for 20ms with 11.0592MHz crystal
    TL0 = 0x0C;   // (65536 - 20000) = 45536 = 0xB80C
    TR0 = 1;      // Enable Timer 0
    while (TF0 == 0)
        ;    // Wait for overflow flag
    TR0 = 0; // Disable Timer 0
    TF0 = 0; // Clear overflow flag
}

void timer_delay30us(void)
{
    /*
     * Function: Create a delay of ~30µs using Timer 0 (mode 1, 16-bit).
     * Application: Used to determine the logic of a data bit (0 or 1) based on the high pulse time.
     * Pulse: After 30µs, read the DHT11 pin status to distinguish between bit 0 (~26-28µs) and bit 1 (~70µs).
     */
    TMOD &= 0xF0; // Timer 0, mode 1 (16-bit)
    TMOD |= 0x01; // Load value for ~30µs
    TH0 = 0xFF;   // (65536 - 30) = 65506 = 0xFFF1
    TL0 = 0xF1;   // Turn on Timer 0
    TR0 = 1;
    while (TF0 == 0)
        ;    // Wait for overflow flag
    TR0 = 0; // Turn off Timer 0
    TF0 = 0; // Clear overflow flag
}

void Request(void)
{
    /*
     * Function: Send request signal to DHT11 to start communication.
     * Pulse:
     * - Pull the DHT11 pin low (0) for 20ms.
     * - Release the pin high (1) to prepare the DHT11 to respond.
     */
    DHT11 = 0;         // Pull the pin low
    timer_delay20ms(); // Create a low pulse for 20ms
    DHT11 = 1;         // Release the pin high
}

void Response(void)
{
    /*
     * Function: Wait and confirm the response signal from DHT11.
     * Pulse:
     * - DHT11 pulls the pin low ~80µs (start response).
     * - Then pulls it high ~80µs (end response).
     * - Wait for another short low pulse before DHT11 sends data.
     */
    while (DHT11 == 1)
        ; // Wait for the pin to go low
    while (DHT11 == 0)
        ; // Wait for pin to go high
    while (DHT11 == 1)
        ; // Wait for pin to go low again
}

int Receive_data(void)
{
    /*
     * Function: Receive 1 byte (8 bits) of data from DHT11.
     * Pulse:
     * - Each bit starts with a low pulse of ~50µs.
     * - The high pulse lasts for ~26-28µs (bit 0) or ~70µs (bit 1).
     * - After ~30µs, read the DHT11 pin status:
     * + If still high (1) -> bit 1.
     * + If low (0) -> bit 0.
     * - Shift left and concatenate each bit to create a complete byte.
     */
    int q, c = 0;
    for (q = 0; q < 8; q++)
    {
        while (DHT11 == 0)
            ;              // Wait for low pulse to end
        timer_delay30us(); // Wait ~30µs to check high pulse length
        if (DHT11 == 1)    // If pin is still high -> bit 1
            c = (c << 1) | 0x01;
        else // If pin is low -> bit 0
            c = (c << 1);
        while (DHT11 == 1)
            ; // Wait for high pulse to end
    }
    return c;
}

void DHT11_Data(unsigned char *strTemp, unsigned char *strRH)
{
    Request();
    Response();
    I_RH = Receive_data();
    D_RH = Receive_data();
    I_Temp = Receive_data();
    D_Temp = Receive_data();
    CheckSum = Receive_data();
    if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
    {
        sprintf(strRH, "ERROR!");
        sprintf(strTemp, "ERROR!");
    }
    else
    {
        sprintf(strRH, "%d.%d", I_RH, D_RH);
        sprintf(strTemp, "%d.%d", I_Temp, D_Temp);
    }
}
