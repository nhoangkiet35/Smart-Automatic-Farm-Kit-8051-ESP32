/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#ifndef LCD_1602_H
#define LCD_1602_H

#include "delay.h"
#include <REGX52.H>

/**
 * @def LCD_DATA_PORT
 * @brief Data port for LCD interface (8-bit mode)
 * @details P0.0 to P0.7 connected to LCD D0-D7 pins
 */
#define LCD_DATA_PORT P0

/**
 * @brief LCD control pins
 */
sbit rs = P2 ^ 6; ///< Register Select (Command/Data)
sbit rw = P2 ^ 5; ///< Read/Write control
sbit en = P2 ^ 7; ///< Enable pulse

/**
 * @brief Send command to LCD
 * @param cmd Command byte to send
 */
void LCD_Command(unsigned char cmd);

/**
 * @brief Write character to LCD
 * @param char_data Character to display
 */
void LCD_Write(unsigned char char_data);

/**
 * @brief Write string to LCD
 * @param str Null-terminated string to display
 */
void LCD_String(unsigned char *str);

/**
 * @brief Write string at specific position
 * @param row 0 for first line, 1 for second line
 * @param position Column position (0-15)
 * @param str String to display
 */
void LCD_String_xy(char row, char position, char *str);

/**
 * @brief Initialize LCD module
 * @details Configures LCD in 8-bit mode, 2 lines, 5x8 font
 */
void LCD_Init(void);

#endif /* LCD_1602_H */