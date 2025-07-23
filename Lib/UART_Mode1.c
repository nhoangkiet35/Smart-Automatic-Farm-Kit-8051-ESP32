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
    SCON = 0x50;  // Mode 1: 8-bit UART, REN enabled (Tx Rx enable)
    TMOD &= 0x0F; // Clear upper 4 bits (Timer1)
    TMOD |= 0x20; // Timer1 Mode 2: 8-bit auto-reload
    TL1 = 0xFD;   // baud = 9600
    TH1 = 0xFD;   // baud = 9600
    TI = 1;       // Set TI to indicate ready to send
    TR1 = 1;      // Start Timer1
}

/**
 * @brief Transmitting 8 bit data
 * @param value Character to transmit.
 *
 * Description: Send 8-bit data while calling this function to send it over UART
 */
void UART_Write(unsigned char value)
{
    SBUF = value;
    while (!TI)
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
        UART_Write(*str++);
}

/**
 * @brief Receives a single character from UART.
 * @return Received character.
 */
char UART_Read(void)
{
    while (!RI)
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
    }
    buffer[i] = '\0'; // Null-terminate
}
