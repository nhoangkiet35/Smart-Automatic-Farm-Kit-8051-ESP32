/*
 * Created Date: Saturday, July 19th 2025, 10:16:57 am
 * Author: Hoang Kiet
 */
#include "UART_Mode1.h"

#define TX_BUFFER_SIZE 16
char tx_buffer[TX_BUFFER_SIZE];
unsigned char tx_head = 0, tx_tail = 0;
bit tx_busy = 0;

/**
 * @brief Initializes UART in Mode 1 using Timer 1 for baud rate generation.
 * Baud rate is calculated based on 11.0592 MHz oscillator.
 * Recommend baud rate is 57600
 */
void UART_Init()
{
    SCON = 0x50 | 0x02; // Mode 1: 8-bit UART, enable TI interrupt, REN enabled (Tx Rx enable)
    TMOD &= 0x0F;       // Clear upper 4 bits (Timer1)
    TMOD |= 0x20;       // Timer1 Mode 2: 8-bit auto-reload

    /* Formula for calculating Timer1 register value for baud rate: TH1=TL1
    TH1 = 256 - (11059200 / (57600 * 32 * 12)) = 0xFF */
    TH1 = 0xFF; // baud rate = 57600
    TL1 = 0XFF; // baud rate = 57600

    ES = 1;  // Enable UART interrupt
    EA = 1;  // Enable global interrupts
    TR1 = 1; // Start Timer1
}
// 0	External Interrupt 0 (INT0)
// 1	Timer 0 Overflow
// 2	External Interrupt 1 (INT1)
// 3	Timer 1 Overflow
// 4	Serial (UART)
void UART_ISR() interrupt 4
{
    if (TI) // Transmit interrupt
    {
        TI = 0;
        if (tx_head != tx_tail) // Buffer not empty
        {
            SBUF = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
        }
        else
            tx_busy = 0; // Transmission complete
    }
}

/**
 * @brief Transmitting 8 bit data
 * @param value Character to transmit.
 */
void UART_Write(char value)
{
    SBUF = value;
    while (!TI)
        ;   // Wait until transmitter is ready
    TI = 0; // Clear transmit flag
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
 * @brief Fast string transmission
 * @param str Pointer to string to transmit.
 */
void UART_Send_String(char *str)
{
    while (*str)
    {
        if ((tx_head + 1) % TX_BUFFER_SIZE != tx_tail) // Buffer not full
        {
            tx_buffer[tx_head] = *str++;
            tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
            if (!tx_busy)
            {
                tx_busy = 1;
                SBUF = tx_buffer[tx_tail];
                tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
            }
        }
        else
        {
            // Buffer full, wait for space (non-blocking)
            while ((tx_head + 1) % TX_BUFFER_SIZE == tx_tail)
                ;
        }
    }
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