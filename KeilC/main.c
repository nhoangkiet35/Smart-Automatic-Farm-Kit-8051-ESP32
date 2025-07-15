#include <REGX52.h>
#include "..\Lib\LCD_1602_KhoiNG.h"
#include "..\Lib\Delay_1ms.h"
#include "..\Lib\UART_Mode1.h"
#include <string.h>
#include <stdio.h>

sbit SoilSensor = P0^0;
sbit Relay = P2^3;
sbit LED1 = P2^0;

char moistureBuffer[6];  // Enough for "100\0"
unsigned char bufferIndex = 0;
bit messageReady = 0;

unsigned char lastState = 0;
unsigned char stableCount = 0;
unsigned char currentState1;
#define DEBOUNCE_LIMIT 5

void main()
{
    LCD_init();
    UART_Init();
    LED1 = 0;
    Relay = 0;

    LCD_string_xy(0, 0, "Moisture: ");
    LCD_string_xy(1, 0, "Status: ----");

    while(1)
    {
        // UART receive buffer
        if (RI)  // If data received
        {
            char c = SBUF;
            RI = 0;

            if (c == '\n')  // End of message
            {
                moistureBuffer[bufferIndex] = '\0';
                messageReady = 1;
                bufferIndex = 0;
            }
            else
            {
                if (bufferIndex < sizeof(moistureBuffer) - 1)
                {
                    moistureBuffer[bufferIndex++] = c;
                }
            }
        }

        // Update LCD with moisture % if new data arrived
        if (messageReady)
        {
            LCD_command(0x01);  // Clear old %
            LCD_string_xy(0, 10, moistureBuffer);
            LCD_string_xy(0, 13, "%");
            messageReady = 0;
        }

        // Digital soil sensor debounce
				currentState1 = SoilSensor;
        if(currentState1 == lastState)
        {
            stableCount++;
        }
        else
        {
            stableCount = 0;
        }

        if (stableCount >= DEBOUNCE_LIMIT)
        {
            LCD_string_xy(1, 8, "         ");  // Clear old status

            if (currentState1 == 1)
            {
                LED1 = 1;
                Relay = 1;
                LCD_string_xy(1, 8, "DRY ON ");
            }
            else
            {
                LED1 = 0;
                Relay = 0;
                LCD_string_xy(1, 8, "WET OFF");
            }

            stableCount = 0;
        }

        lastState = currentState1;
        Delay_1ms(500);
    }
}
