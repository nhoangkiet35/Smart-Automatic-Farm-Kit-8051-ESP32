/*
 * Created Date: Saturday, July 19th 2025, 10:16:57 am
 * Author: Hoang Kiet
 */
#include "UART_Mode1.h"

/**
 * @brief Initializes UART in Mode 1 using Timer 1 for baud rate generation.
 * Baud rate is calculated based on 11.0592 MHz oscillator.
 *
 * @param baudrate Desired UART baud rate (e.g., 9600, 4800).
 */
void UART_Init()
{
    unsigned int baudrate = 9600;
    unsigned int reload_value;

    SCON = 0x50;  // Mode 1: 8-bit UART, REN enabled
    TMOD &= 0x0F; // Clear upper 4 bits (Timer1)
    TMOD |= 0x20; // Timer1 Mode 2: 8-bit auto-reload

    // Calculate TH1 reload value for 11.0592 MHz clock
    reload_value = 256 - (11059200UL / (12UL * 32UL * baudrate));
    TH1 = (unsigned char)reload_value;

    TR1 = 1; // Start Timer1
    TI = 1;  // Set TI to indicate ready to send
}

/**
 * @brief Sends a single character over UART.
 * @param c Character to transmit.
 */
void UART_Write(char c)
{
    SBUF = c;
    while (TI == 0)
        ;
    TI = 0;
}

/**
 * @brief Sends a null-terminated string over UART.
 * @param str Pointer to string to transmit.
 */
void UART_Write_String(char *str)
{
    while (*str)
    {
        UART_Write(*str++);
    }
}

/**
 * @brief Receives a single character from UART.
 * @return Received character.
 */
char UART_Read(void)
{
    while (RI == 0)
        ; // Wait for character
    RI = 0;
    return SBUF;
}

/**
 * @brief Reads a string from UART up to newline or max_length.
 * @param buffer Buffer to store received string.
 * @param max_length Maximum number of characters to read.
 */
void UART_Read_String(char *buffer, unsigned char max_length)
{
    unsigned char i = 0;
    char c;

    while (i < (max_length - 1))
    {
        c = UART_Read();
        if (c == '\r' || c == '\n')
            break;
        buffer[i++] = c;
        UART_Write(c); // Optional: echo character
    }

    buffer[i] = '\0'; // Null-terminate
}
