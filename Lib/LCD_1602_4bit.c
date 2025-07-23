/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#include "LCD_1602_4bit.h"
#include "delay.h"

void LCD_Command(unsigned char cmd)
{
    //  LCD_DATA_PORT = cmd;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmd & 0xF0); // Gi? nguy�n 4 bit th?p c?a P0, g�n 4 bit cao c?a l?nh
                                                           // Register Select
    rs = 0;                                                // Ch? d? l?nh
                                                           // Read/Write
    rw = 0;                                                // 0 = write operation, 1 = read operation
                                                           // Enable
    en = 1;                                                // K�ch ho?t
    delay(1);
    en = 0;

    // G?i 4 bit th?p sau
    LCD_DATA_PORT =
        (LCD_DATA_PORT & 0x0F) | ((cmd << 4) & 0xF0); // Gi? nguy�n 4 bit th?p c?a P0, g�n 4 bit th?p c?a l?nh
    en = 1;                                           // K�ch ho?t
    delay(1);
    en = 0;

    delay(5); // Ch? LCD x? l�
}

void LCD_Write(unsigned char char_data)
{
    //  LCD_DATA_PORT = char_data;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (char_data & 0xF0); // G?i 4 bit cao tru?c
    rs = 1;                                                      // Ch? d? d? li?u
    rw = 0;                                                      // 0 = write operation, 1 = read operation
    en = 1;                                                      // K�ch ho?t
    delay(1);
    en = 0;

    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((char_data << 4) & 0xF0); // Gi? 4 bit th?p sau
    en = 1;                                                             // K�ch ho?t
    delay(1);
    en = 0;

    delay(5); // Ch? LCD x? l�
}

void LCD_String(unsigned char *str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        LCD_Write(str[i]); // data write on LCD
    }
}
void LCD_String_xy(char row, char position, char *str)
{
    if (row == 0)
        LCD_Command((position & 0x0F) | 0x80); // L?nh d?t con tr? h�ng 0
    else if (row == 1)
        LCD_Command((position & 0x0F) | 0xC0); // L?nh d?t con tr? h�ng 1
    LCD_String(str);                           // G?i LCD_String d? hi?n th? chu?i sau khi d?t v? tr�
}

void LCD_Init(void)
{
    delay(20); // Ch? LCD kh?i d?ng

    // Kh?i t?o LCD 4-bit mode
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x30; // G?i 0x3 (0011) 3 l?n d? ch?c ch?n chuy?n sang 8-bit mode tru?c
    rs = 0;                                        // Ch? d? l?nh
    rw = 0;                                        // Ghi
    en = 1;
    delay(1);
    en = 0;
    delay(5);
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x30;
    en = 1;
    delay(1);
    en = 0;
    delay(5);
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x30;
    en = 1;
    delay(1);
    en = 0;
    delay(5);

    // B�y gi? g?i l?nh chuy?n sang 4-bit mode
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x20; // G?i 0x20 (0010) d? chuy?n sang 4-bit mode
    en = 1;
    delay(1);
    en = 0;
    delay(5);

    // T? d�y tr? di, h�m LCD_Command d� x? l� vi?c g?i 2 nibble
    LCD_Command(0x28); // Function Set: 4-bit mode, 2 d�ng, font 5x8
    LCD_Command(0x0C); // Display On, Cursor Off, Blink Off
    LCD_Command(0x06); // Entry Mode Set: Increment cursor, No shift
    LCD_Command(0x01); // Clear Display (v� d?t con tr? v? 0,0)
    LCD_Command(0x80); // Set DDRAM Address to 0x00 (H�ng 0, C?t 0)
}

void MyLCDDemo()
{                             // �?i t�n h�m LCD() th�nh MyLCDDemo() d? tr�nh nh?m l?n
    LCD_Init();               /* initialization of LCD*/
    LCD_String("Hoang Kiet"); /* write string on 1st line of LCD*/
    LCD_Command(0xC0);
    LCD_String("Hello World"); /*write string on 2nd line*/
}