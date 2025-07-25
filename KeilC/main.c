#include <REGX52.H>
#include <stdio.h>
#include <string.h>

#include "..\Lib\DHT11.h"
#include "..\Lib\LCD_1602.h"
#include "..\Lib\SoilMoisture.h"
#include "..\Lib\UART_Mode1.h"
#include "..\Lib\WaterLevel.h"
#include "..\Lib\beep.h"
#include "..\Lib\delay.h"

/* NOTE: My kit 8051 is operating in active-low mode. */
#define TURN_ON  0
#define TURN_OFF 1

/* Define pins for L298N */
sbit PUMP_CONTROL = P2 ^ 0; // IN1 for pump control
sbit FAN_CONTROL  = P2 ^ 2; // IN3 for fan control

/* Define button pins */
sbit BUTTON_TEMP_HUM = P3 ^ 0; // Button for temperature and humidity display
sbit BUTTON_PUMP     = P3 ^ 1; // Button for pump control
sbit BUTTON_FAN      = P3 ^ 2; // Button for fan control
sbit BUTTON_ALERT    = P3 ^ 3; // Button for low water level alert

/* Timer variables for UART transmission */
unsigned int uart_timer = 0;
#define UART_SEND_INTERVAL 100  // Send every 10 seconds (100 * 100ms = 10000ms)

// LCD display management
unsigned int lcd_timer = 0;
unsigned char lcd_buffer[16];
#define LCD_RESTORE_TIME 30     // Restore LCD after 3 seconds (30 * 100ms)
bit lcd_showing_uart_status = 0;

// UART management
bit uart_transmission_active = 0;
unsigned char uart_char_index = 0;
char uart_buffer[64];

/* Main function */
void main(void)
{
    unsigned int soil_moisture, water_level, temperature, humidity;
    

    // Initialize peripherals
    SoilMoisture_Init();
    WaterLevel_Init();
    LCD_Init();
    UART_Init();
    delay(100); // Wait for system stabilization

    // Configure L298N control pins as output
    PUMP_CONTROL = TURN_OFF; // Initially off
    FAN_CONTROL  = TURN_OFF; // Initially off
    buzzer       = TURN_OFF; // Initially off

    // Initialize timer
    uart_timer = 0;
	lcd_timer = 0;
    uart_transmission_active = 0;
    lcd_showing_uart_status = 0;

    // Display initialization message on LCD
    LCD_String_xy(0, 0, "Smart Farm Kit ");
    LCD_String_xy(1, 0, "Initializing...");
    delay(2000);
    LCD_Command(0x01); // Clear LCD

    while (1) {
        /* Read sensor data */
        soil_moisture = getSoilMoisture(); // 0-100
        water_level   = getWaterLevel();   // 1, 50, 100
        temperature   = getTemperature();  // ℃
        humidity      = getHumidity();     // %

        /* DEBUG */
        LCD_String_xy(0, 0, "Enter a number: ");
		sprintf(lcd_buffer,"/> #1 #2 #3 #4 %d", uart_timer);
        LCD_String_xy(1, 0, lcd_buffer);

        /* Handling button and sensor */
        // Button 1 (P3.0): Display on LCD temperature and humidity
        if (BUTTON_TEMP_HUM == 0) // Active-low
        {
            LCD_Command(0x01);        // Clear LCD
            delay(50);                // Debounce
            if (BUTTON_TEMP_HUM == 0) // Confirm press
            {
                // Print on LCD with temp/humidity
                getInfo_TemperatureAndHumidity(lcd_buffer);
                while (BUTTON_TEMP_HUM == 0); // Wait for release
            }
        }

        // Button 2 (P3.1): Display control pump on LCD
        if (BUTTON_PUMP == 0) {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_PUMP == 0) {
                LCD_String_xy(0, 0, "Control Pump:");
                sprintf(lcd_buffer, "Soil: %s", soil_moisture ? "wet" : "dry");
                LCD_String_xy(1, 0, lcd_buffer);
                if (soil_moisture <= 30 && water_level > 1) // Dry soil and water available
                    LCD_String_xy(1, 13, " ON");
                else {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                while (BUTTON_PUMP == 0); // Wait for release
            }
        }
        // Target: Control pump
        if (soil_moisture <= 30 && water_level > 1) // Dry soil and water available
            PUMP_CONTROL = TURN_ON;
        else
            PUMP_CONTROL = TURN_OFF; // Cannot turn on pump

        // Button 3 (P3.2): Display control fan on LCD
        if (BUTTON_FAN == 0) {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_FAN == 0) {
                LCD_String_xy(0, 0, "Control Fan:");
                sprintf(lcd_buffer, "Temp: %s", temperature >= 35 ? "high" : "stable");
                LCD_String_xy(1, 0, lcd_buffer);
                if (temperature >= 35) // High temperature
                    LCD_String_xy(1, 13, " ON");
                else {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                while (BUTTON_FAN == 0); // Wait for release
            }
        }
        // Target: Control fan
        if (temperature >= 35) // High temperature
            FAN_CONTROL = TURN_ON;
        else
            FAN_CONTROL = TURN_OFF; // Cannot turn on fan

        // Button 4 (P3.3): Low water level alert
        if (BUTTON_ALERT == 0) {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_ALERT == 0) {
                LCD_String_xy(0, 0, "Alert Level:");
                if (water_level == 1) {
                    LCD_String_xy(1, 0, "Empty");
                    beep(500, 3); // Beep three times
                } else if (water_level == 50) {
                    LCD_String_xy(1, 0, "Half");
                } else if (water_level == 100) {
                    LCD_String_xy(1, 0, "Full");
                }
                while (BUTTON_ALERT == 0); // Wait for release
            }
        }

        /* Send data to ESP32 via UART once 10s for ThingsBoard */
        // Send UART data with 4 variables "soil:%d,temp:%d,hum:%d,water:%d"
        uart_timer++;
		if (uart_timer >= UART_SEND_INTERVAL) {
            uart_timer = 0; // Reset timer

            // Format JSON data for ESP32
            sprintf(uart_buffer, "{\"soil\":%d,\"temp\":%d,\"hum\":%d,\"water\":%d}\n",
                    soil_moisture, temperature, humidity, water_level);
            // Send via UART
            UART_Send_String(uart_buffer);

            // Display sending status on LCD briefly
            LCD_Command(0x01);
            LCD_String_xy(0, 0, "Data sent to");
            LCD_String_xy(1, 0, "ESP32 via UART");
        }

        // Delay before next iteration
        delay(100); // Short delay to keep system responsive
    }
}