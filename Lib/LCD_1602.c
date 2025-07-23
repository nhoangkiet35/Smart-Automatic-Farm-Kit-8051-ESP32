/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#include "LCD_1602.h"

/**
 * @brief Send command to LCD
 * @param cmd Command byte to send
 * @details Sets RS low for command mode, RW low for write operation
 *          Generates enable pulse with proper timing
 */
void LCD_Command(unsigned char cmd)
{
    LCD_DATA_PORT = cmd; // Load command to data port
    rs = 0;              // Command mode (RS=0)
    rw = 0;              // Write operation (RW=0)
    en = 1;              // Generate high pulse
    delay(1);            // Wait for 1ms
    en = 0;              // Complete the pulse
    delay(5);            // Wait for command processing
}

/**
 * @brief Write character to LCD
 * @param char_data Character to display
 * @details Sets RS high for data mode, RW low for write operation
 *          Generates enable pulse with proper timing
 */
void LCD_Write(unsigned char char_data)
{
    LCD_DATA_PORT = char_data; // Load character to data port
    rs = 1;                    // Data mode (RS=1)
    rw = 0;                    // Write operation (RW=0)
    en = 1;                    // Generate high pulse
    delay(1);                  // Wait for 1ms
    en = 0;                    // Complete the pulse
    delay(5);                  // Wait for data processing
}

/**
 * @brief Write string to LCD
 * @param str Null-terminated string to display
 * @details Writes each character until null terminator is found
 */
void LCD_String(unsigned char *str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        LCD_Write(str[i]); // Write each character
    }
}

/**
 * @brief Write string at specific position
 * @param row 0 for first line, 1 for second line
 * @param position Column position (0-15)
 * @param str String to display
 * @details Sets cursor position before writing string
 */
void LCD_String_xy(char row, char position, char *str)
{
    if (row == 0)
        LCD_Command((position & 0x0F) | 0x80); // First line address
    else if (row == 1)
        LCD_Command((position & 0x0F) | 0xC0); // Second line address
    LCD_String(str);                           // Write the string
}

/**
 * @brief Initialize LCD module
 * @details Performs LCD initialization sequence:
 *          1. Wait for LCD power stabilization
 *          2. Set 8-bit mode, 2 lines, 5x8 font
 *          3. Turn display on, cursor off
 *          4. Set entry mode (increment cursor)
 *          5. Clear display
 *          6. Set cursor to home position
 */
void LCD_Init(void)
{
    delay(20); // Wait for LCD power up

    // Function Set: 8-bit, 2 lines, 5x8 font
    LCD_Command(0x38);

    // Display On/Off: Display on, cursor off, blink off
    LCD_Command(0x0C);

    // Entry Mode: Increment cursor, no display shift
    LCD_Command(0x06);

    // Clear display
    LCD_Command(0x01);

    // Set DDRAM address to 0 (cursor home)
    LCD_Command(0x80);
}