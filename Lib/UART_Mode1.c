/*
 * Universal Asynchronous Receiver-Transmitter (UART)
 * Created Date: Saturday, July 19th 2025, 10:16:57 am
 * Author: Hoang Kiet
 *
 * Description: Sent by UART_Send_String -> Push into buffer -> ISR transmits gradually
 */
#include "UART_Mode1.h"
#include <string.h> // Add memset() function

#define TX_BUFFER_SIZE 16
char tx_buffer[TX_BUFFER_SIZE];
unsigned char tx_buffer_index;

/**
 * @brief Initializes UART in Mode 1 using Timer 1 for baud rate generation.
 * Baud rate is calculated based on 11.0592 MHz oscillator.
 * Recommend baud rate is 57600
 */
void UART_Init()
{
    SCON = 0x50;  // Mode 1: 8-bit UART, REN enabled
    TMOD &= 0x0F; // Clear Timer1 bits
    TMOD |= 0x20; // Timer1 Mode 2 (8-bit auto-reload)

    /* Formula for calculating Timer1 register value for baud rate: TH1=TL1
        TH1 = 256 - (11059200 / (9600 * 32 * 12)) */
    TH1 = 0xFD; // 9600 baud rate @ 11.0592 MHz
    TL1 = TH1;
    TR1 = 1; // Start Timer1
    TI = 1;  // Set TI flag to ready for first transmit
}

void Ext_Interrupt_Init()
{
    EA = 1; /* Enable global interrupt */
    ES = 1; /* Enable serial interrupt */
}

// 0	External Interrupt 0 (INT0)     // 1	Timer 0 Overflow
// 2	External Interrupt 1 (INT1)     // 3	Timer 1 Overflow    // 4	Serial (UART)
void UART_ISR() interrupt 4
{
    if (TI == 1) // Transmit interrupt
    {
        if (tx_buffer[tx_buffer_index] != 0) // Buffer not empty
            SBUF = tx_buffer[tx_buffer_index++];
        TI = 0; /* Clear TI flag */
    }
}

/**
 * @brief Transmitting 8 bit data
 * @param ch Character to transmit.
 */
void UART_Write(char ch)
{
    SBUF = ch; // Load char in SBUF register
    while (TI == 0)
        ;   // Wait until transmitter is ready
    TI = 0; // Clear transmit flag
}

/**
 * @brief Sends a null-terminated string over UART.
 * @param str Pointer to string to transmit.
 */
void UART_Write_String(char *str)
{
    while (*str)            // Send each char of string till the NULL
        UART_Write(*str++); // Call transmit data function
}

/**
 * @brief Fast string transmission
 * @param str Pointer to string to transmit.
 */
void UART_Send_String(char *str)
{
    char i;
    memset(tx_buffer, 0, TX_BUFFER_SIZE);
    for (i = 0; str[i] != 0 && i < TX_BUFFER_SIZE; i++)
    {
        tx_buffer[i] = str[i];
    }
    tx_buffer[15] = 0;
    SBUF = tx_buffer[0];
    tx_buffer_index = 1;
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

/**
 * @brief Clear UART buffers and reset transmission state
 */
void UART_Reset(void)
{
    TI = 1; // Set transmit flag ready
    RI = 0; // Clear receive flag
}