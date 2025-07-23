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

//* NOTE: My kit 8051 is operating in active-low mode.
#define TURN_ON 0
#define TURN_OFF 1

// Define pins for L298N
sbit PUMP_CONTROL = P2 ^ 0; // IN1 for pump control
sbit FAN_CONTROL = P2 ^ 2;  // IN3 for fan control

// Define button pins
sbit BUTTON_TEMP_HUM = P3 ^ 0; // Button for temperature and humidity display
sbit BUTTON_PUMP = P3 ^ 1;     // Button for pump control
sbit BUTTON_FAN = P3 ^ 2;      // Button for fan control
sbit BUTTON_ALERT = P3 ^ 3;    // Button for low water level alert

// Define variables
unsigned long last_update = 0;

// Function prototypes
unsigned char debounce_button(sbit, unsigned long *, unsigned char);

// Main function
void main(void)
{
    unsigned int soil_moisture, water_level, temperature, humidity;
    unsigned char lcd_buffer[16], buffer[40];

    // Initialize peripherals
    SoilMoisture_Init();
    WaterLevel_Init();
    LCD_Init();
    UART_Init();
    Timer0_Init();
    delay(100); // Wait for system stabilization

    // Configure L298N control pins as output
    PUMP_CONTROL = TURN_OFF; // Initially off
    FAN_CONTROL = TURN_OFF;  // Initially off
    buzzer = TURN_OFF;       // Initially off

    // Display initialization message on LCD
    LCD_String_xy(0, 0, "Smart Farm Kit ");
    LCD_String_xy(1, 0, "Initializing...");
    delay(2000);
    LCD_Command(0x01); // Clear LCD

    while (1)
    {
        // Read sensor data
        soil_moisture = getSoilMoisture(); // 0-100
        water_level = getWaterLevel();     // 1, 50, 100
        temperature = getTemperature();    // ℃
        humidity = getHumidity();          // %

        /* Send data to ESP32 via UART for ThingsBoard */
        // Check every 5 seconds
        if (Timer0_GetMillis() - last_update >= 5000)
        {
            last_update = Timer0_GetMillis();
            // Send UART data with 4 variables
            sprintf(buffer, "soil:%d,temp:%d,hum:%d,water:%d", soil_moisture, temperature, humidity, water_level);
            UART_Write_String(buffer);
            UART_Write('\n');
        }

        /* DEBUG */
        LCD_String_xy(0, 0, "Enter a number: ");
        LCD_String_xy(1, 0, "/> #1 #2 #3 #4  ");

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
                while (BUTTON_TEMP_HUM == 0)
                    ; // Wait for release
            }
        }

        // Button 2 (P3.1): Display on LCD
        if (BUTTON_PUMP == 0)
        {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_PUMP == 0)
            {
                LCD_String_xy(0, 0, "Control Pump:");
                sprintf(lcd_buffer, "Soil: %s", soil_moisture ? "wet" : "dry");
                LCD_String_xy(1, 0, lcd_buffer);
                if (soil_moisture <= 30 && water_level > 1) // Dry soil and water available
                    LCD_String_xy(1, 13, " ON");
                else
                {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                while (BUTTON_PUMP == 0)
                    ; // Wait for release
            }
        }
        // Target: Control pump
        if (soil_moisture <= 30 && water_level > 1) // Dry soil and water available
            PUMP_CONTROL = TURN_ON;
        else
            PUMP_CONTROL = TURN_OFF; // Cannot turn on pump

        // Button 3 (P3.2):
        if (BUTTON_FAN == 0)
        {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_FAN == 0)
            {
                LCD_String_xy(0, 0, "Control Fan:");
                sprintf(lcd_buffer, "Temp: %s", temperature >= 35 ? "high" : "stable");
                LCD_String_xy(1, 0, lcd_buffer);
                if (temperature >= 35) // High temperature
                    LCD_String_xy(1, 13, " ON");
                else
                {
                    LCD_String_xy(1, 13, "OFF");
                    beep(250, 2); // Beep to indicate error
                }
                while (BUTTON_FAN == 0)
                    ; // Wait for release
            }
        }
        // Target: Control fan
        if (temperature >= 35) // High temperature
            FAN_CONTROL = TURN_ON;
        else
            FAN_CONTROL = TURN_OFF; // Cannot turn on fan

        // Button 4 (P3.3): Low water level alert
        if (BUTTON_ALERT == 0)
        {
            LCD_Command(0x01); // Clear LCD
            delay(50);         // Debounce
            if (BUTTON_ALERT == 0)
            {
                LCD_String_xy(0, 0, "Alert Level:");
                if (water_level == 1)
                {
                    LCD_String_xy(1, 0, "Empty");
                    beep(500, 3); // Beep three times
                }
                else if (water_level == 50)
                {
                    LCD_String_xy(1, 0, "Half");
                }
                else if (water_level == 100)
                {
                    LCD_String_xy(1, 0, "Full");
                }
                while (BUTTON_ALERT == 0)
                    ; // Wait for release
            }
        }

        // Delay before next iteration
        delay(3000); // Short delay to keep system responsive
    }
}

unsigned char debounce_button(sbit button, unsigned long *last_time, unsigned char id)
{
    static unsigned char last_state[4] = {1, 1, 1, 1}; // Assume active-low
    unsigned char current_state = button;
    unsigned long current_time = Timer0_GetMillis();

    if (current_state != last_state[id] && (current_time - *last_time) > 50)
    {
        *last_time = current_time;
        last_state[id] = current_state;
        if (current_state == 0)
            return 1; // button pressed
    }
    return 0;
}
