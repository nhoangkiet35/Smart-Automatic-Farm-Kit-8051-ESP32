/*
 * Created Date: Wednesday, July 16th 2025, 3:08:31 pm
 * Author: Hoang Kiet
 *
 * Purpose: Implements UART communication in Mode 1 (8-bit, variable baud rate) for the 8051 microcontroller
 * to communicate with the ESP32.
 */
#ifndef _UART_MODE1_H_
#define _UART_MODE1_H_

#include <REGX52.H>

/**
 * @brief Initializes UART in Mode 1 (8-bit, variable baud rate).
 * @param baudrate Desired baud rate (e.g., 9600, 4800). Use standard values.
 *        Assumes 11.0592 MHz system clock.
 */
void UART_Init();

/**
 * @brief Sends a single character via UART.
 * @param c The character to send.
 */
void UART_Write(char c);

/**
 * @brief Sends a null-terminated string via UART.
 * @param str Pointer to the string to send.
 */
void UART_Write_String(char *str);

/**
 * @brief Receives a single character via UART.
 * @return The received character.
 */
char UART_Read(void);

/**
 * @brief Reads a string from UART until newline, carriage return, or max length.
 * @param buffer Pointer to buffer to store received string.
 * @param max_length Maximum characters to read (including null terminator).
 */
void UART_Read_String(char *buffer, unsigned char max_length);

#endif // _UART_MODE1_H_
