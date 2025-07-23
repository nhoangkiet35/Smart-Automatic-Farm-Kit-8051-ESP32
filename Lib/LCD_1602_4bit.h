/*
 * Created Date: Wednesday, July 16th 2025, 3:06:29 pm
 * Author: Hoang Kiet
 */
#ifndef _LCD_1602_4BIT_H_ // Th�m include guards d? tr�nh include nhi?u l?n
#define _LCD_1602_4BIT_H_

#include <REGX52.H> // C� th? c?n reg51.h ? d�y n?u c�c sbit d?nh nghia trong header

// �?nh nghia c�c ch�n di?u khi?n v� c?ng d? li?u
#define LCD_DATA_PORT P0
sbit rs = P2 ^ 6;
sbit rw = P2 ^ 5;
sbit en = P2 ^ 7;

// Khai b�o c�c h�m LCD
void delay(unsigned int count);
void LCD_Command(unsigned char cmd);
void LCD_Write(unsigned char char_data);
void LCD_String(unsigned char *str);
void LCD_String_xy(char row, char position, char *str);
void LCD_Init(void);

void MyLCDDemo(void);

#endif // _LCD_1602_4BIT_H_