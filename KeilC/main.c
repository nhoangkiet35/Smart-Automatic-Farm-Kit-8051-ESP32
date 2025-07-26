/*
 * Project: ESP32 MQTT + ThingsBoard Smart Automatic Farm Kit 8051-ESP32
 * Created Date: Wednesday, July 5th 2025, 3:39:18 pm
 */

#include <REGX52.H>
#include <stdio.h>
#include <MATH.H>

#include "..\Lib\DHT11.h"
#include "..\Lib\LCD_1602.h"
#include "..\Lib\SoilMoisture.h"
#include "..\Lib\UART_Mode1.h"
#include "..\Lib\WaterLevel.h"
#include "..\Lib\beep.h"
#include "..\Lib\delay.h"

/* NOTE: My 8051 is operating in active-low mode. */
#define TURN_ON  0
#define TURN_OFF 1

/* Define pins for L298N */
sbit PUMP_CONTROL = P2 ^ 0; // IN1 for pump control
sbit FAN_CONTROL  = P2 ^ 2; // IN3 for fan control

/* Define button matrix 4x4 */
#define ROW1 P1_7
#define ROW2 P1_6
#define ROW3 P1_5
#define ROW4 P1_4
#define COL1 P1_3
#define COL2 P1_2
#define COL3 P1_1
#define COL4 P1_0

/* Define button pins */
#define BUTTON_TEMP_HUM 1 // Row 4, Col 1 // Button for temperature and humidity display
#define BUTTON_PUMP     2 // Row 4, Col 2 // Button for pump control
#define BUTTON_FAN      3 // Row 4, Col 3 // Button for fan control
#define BUTTON_ALERT    4 // Row 4, Col 4 // Button for low water level alert

/* Timer variables for UART transmission */
#define UART_SEND_INTERVAL 10 // Send every 30 seconds (15 * 2000ms = 30s)
// static unsigned int last_soil = 0, last_temp = 0, last_hum = 0, last_water = 0;
unsigned int uart_timer = 0;

/* Shared buffer for LCD and UART */
unsigned char shared_buffer[16];
unsigned char key;

/* Function prototypes */
unsigned char scan_keypad(void);

/* Main function */
void main(void)
{
    unsigned int soil_moisture, water_level, temperature, humidity;

    // Initialize matrix variables
    P1 = 0xFF; // 1111 1111 // Row is high, column have internal pull-up

    // Initialize peripherals
    // Ext_Interrupt_Init(); /* Call Ext. interrupt initialize */
    LCD_Init();
    UART_Init();
    delay(100); // Wait for system stabilization

    // Configure L298N control pins as output
    PUMP_CONTROL = TURN_OFF; // Initially off
    FAN_CONTROL  = TURN_OFF; // Initially off

    // Display initialization message on LCD
    LCD_String_xy(0, 0, "Smart Farm Kit  ");
    LCD_String_xy(1, 0, "Initializing... ");
    delay(2000);
    LCD_Command(0x01); // Clear LCD

    while (1) {
        /* Read sensor data periodically */
        soil_moisture = getSoilMoisture(); // 0-100
        water_level   = getWaterLevel();   // 1, 50, 100
        temperature   = getTemperature();  // ℃
        humidity      = getHumidity();     // %

        /* PASSED Handling button and sensor */
        key = scan_keypad(); // Active-low
        switch (key) {
            // Button 1 (P1.4-P1.3): Display on LCD temperature and humidity
            case BUTTON_TEMP_HUM:
                LCD_Command(0x01); // Clear LCD
                sprintf(shared_buffer, "Temp: %d.%d *C", I_Temp, D_Temp);
                LCD_String_xy(0, 0, shared_buffer);
                sprintf(shared_buffer, "Hum : %d.%d", I_RH, D_RH);
                LCD_String_xy(1, 0, shared_buffer);
                LCD_String_xy(1, 11, "%");
                break;
            // Button 2 (P1.4-P1.2): Display control pump on LCD
            case BUTTON_PUMP:
                LCD_Command(0x01); // Clear LCD
                LCD_String_xy(0, 0, "Control Pump:");
                sprintf(shared_buffer, "Soil: %s", soil_moisture ? "wet" : "dry");
                LCD_String_xy(1, 0, shared_buffer);
                // Dry soil and water available
                if (soil_moisture <= 30 && water_level > 1)
                    LCD_String_xy(1, 13, " ON");
                else {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                break;
            // Button 3 (P1.4-P1.1): Display control fan on LCD
            case BUTTON_FAN:
                LCD_Command(0x01); // Clear LCD
                LCD_String_xy(0, 0, "Control Fan:");
                sprintf(shared_buffer, "Temp: %s", temperature >= 35 ? "high" : "stable");
                LCD_String_xy(1, 0, shared_buffer);
                if (temperature >= 35) // High temperature
                    LCD_String_xy(1, 13, " ON");
                else {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                break;
            // Button 4 (P1.4-P1.0): Low water level alert
            case BUTTON_ALERT:
                LCD_Command(0x01); // Clear LCD
                LCD_String_xy(0, 0, "Alert Level:");
                if (water_level == 1) {
                    LCD_String_xy(1, 0, "Empty");
                    beep(500, 3); // Beep three times
                } else if (water_level == 50) {
                    LCD_String_xy(1, 0, "Half");
                } else if (water_level == 100) {
                    LCD_String_xy(1, 0, "Full");
                }
                break;
            default:
                /* PENDING Handle LCD display restoration */
                LCD_String_xy(0, 0, "Enter a number: ");
                sprintf(shared_buffer, "> #1 #2 #3 #4 %2d", uart_timer);
                LCD_String_xy(1, 0, shared_buffer);
                break;
        }

        // TARGET: Automatic act pump
        if (soil_moisture <= 30 && water_level > 1) // Dry soil and water available
            PUMP_CONTROL = TURN_ON;
        else
            PUMP_CONTROL = TURN_OFF; // Cannot turn on pump

        // TARGET: Automatic act fan
        if (temperature >= 35) // High temperature
            FAN_CONTROL = TURN_ON;
        else
            FAN_CONTROL = TURN_OFF; // Cannot turn on fan

        /* DEBUG Send data to ESP32 via UART once 20s */
        // Send string UART data with 4 variables "soil,temp,hum,water"
        if (uart_timer == UART_SEND_INTERVAL) {
            //     // if (abs(soil_moisture - last_soil) > 5 || abs(temperature - last_temp) > 1 ||
            //     //     abs(humidity - last_hum) > 5 || abs(water_level - last_water) > 10) {
            // Format data for ESP32
            snprintf(shared_buffer, 10, "%u,%u,%u,%u", soil_moisture, temperature, humidity, water_level);
            // UART_Write_String("\n0,34,50,1\r\n"); // Send via UART
            UART_Send_String("0,34,50,1\r\n"); // Send via UART "xxx,xx,xx,xxx"
            // last_soil  = soil_moisture;
            // last_temp  = temperature;
            // last_hum   = humidity;
            // last_water = water_level;
            // Display sending status on LCD briefly
            LCD_Command(0x01); // Clear LCD
            LCD_String_xy(0, 0, shared_buffer);
            LCD_String_xy(1, 0, "ESP32 via UART  ");
            //    // }
            uart_timer = 0; // Reset timer
            UART_Reset();   // Reset flag of uart
        }
        uart_timer++;
        // Delay before next iteration
        delay(2000); // Short delay to keep system responsive
    }
}

unsigned char scan_keypad(void)
{
    // Scan Row 4 // Active-low
    ROW1 = 1;
    ROW2 = 1;
    ROW3 = 1;
    ROW4 = 0;
    delay(2);
    if (COL1 == 0) {
        delay(20); // Debounce
        // Confirm press
        if (COL1 == 0) return BUTTON_TEMP_HUM;
    }
    if (COL2 == 0) {
        delay(20);
        if (COL2 == 0) return BUTTON_PUMP;
    }
    if (COL3 == 0) {
        delay(20);
        if (COL3 == 0) return BUTTON_FAN;
    }
    if (COL4 == 0) {
        delay(20);
        if (COL4 == 0) return BUTTON_ALERT;
    }
    return 0; // No button pressed
}
