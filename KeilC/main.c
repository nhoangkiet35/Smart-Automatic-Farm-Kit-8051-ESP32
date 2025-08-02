/*
 * Project: ESP32 MQTT + ThingsBoard Smart Automatic Farm Kit 8051-ESP32
 * Created Date: Wednesday, July 5th 2025, 3:39:18 pm
 */
#include <REGX52.H>
#include <stdio.h>
#include "STDLIB.H"

#include "..\Lib\DHT11.h"
#include "..\Lib\LCD_1602.h"
#include "..\Lib\UART_Mode1.h"
#include "..\Lib\beep.h"
#include "..\Lib\delay.h"

/* NOTE: My 8051 is operating in active-low mode. */
#define TURN_ON  0
#define TURN_OFF 1

/* Define pins for L298N */
sbit PUMP_CONTROL = P2 ^ 0; // IN1 for pump control
sbit FAN_CONTROL  = P2 ^ 2; // IN3 for fan control

/* Define digital pins as input */
sbit WATER_LEVEL_MIN_PIN = P1 ^ 5; // Half level sensor
sbit WATER_LEVEL_MAX_PIN = P1 ^ 6; // Full level sensor
sbit SOIL_MOISTURE_PIN   = P2 ^ 4; // Digital pin D0

/* Define button matrix 4x4 */
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
// #define UART_SEND_INTERVAL 5 // Send every 30 seconds (15 * 2000ms = 30s)
// unsigned char uart_timer = 0;

/* Function prototypes */
unsigned int getWaterLevel(void);
unsigned int getSoilMoisture(void);

unsigned char scan_keypad(void);
void display_common(const char *, const char *, const char *);

/* Main function */
void main(void)
{
    /* Shared buffer for LCD and UART */
    unsigned char shared_buffer[16], temp[6], hum[6], key;
    unsigned int soil = 0, water = 0;

    // Initialize peripherals
    LCD_Init();
    UART_Init();
    delay(100); // Wait for system stabilization

    // Configure L298N control pins as output
    PUMP_CONTROL = TURN_OFF; // Initially off
    FAN_CONTROL  = TURN_OFF; // Initially off

    // Initialize matrix variables
    P1 = 0xFF; // 1111 1111 // All high

    // Display initialization message on LCD
    display_common("Smart Farm Kit", "Initializing...", "");
    delay(2000);
    LCD_Command(0x01); // Clear LCD

    while (1) {
        /* PASSED Read sensor data periodically */
        DHT11_Data(temp, hum);     // ℃, %
        soil  = getSoilMoisture(); // 0-100
        water = getWaterLevel();   // 1, 50, 100

        /* PASSED Handling button and sensor */
        key = scan_keypad(); // Active-low
        switch (key) {
            //* Button 1 (P1.4-P1.3): Display on LCD temperature and humidity
            case BUTTON_TEMP_HUM:
                LCD_Command(0x01); // Clear LCD
                sprintf(shared_buffer, "Temp: %s *C", temp);
                LCD_String_xy(0, 0, shared_buffer);
                sprintf(shared_buffer, "Hum : %s", hum);
                LCD_String_xy(1, 0, shared_buffer);
                LCD_String(" %");
                break;
            //* Button 2 (P1.4-P1.2): Display control pump on LCD
            case BUTTON_PUMP:
                display_common("Control Pump:",
                               soil ? "Soil: wet" : "Soil: dry",
                               (soil <= 30 && water > 1) ? "ON" : "OFF");
                break;
            //* Button 3 (P1.4-P1.1): Display control fan on LCD
            case BUTTON_FAN:
                display_common("Control Fan:",
                               atoi(temp) >= 35 ? "Temp: high" : "Temp: stable",
                               atoi(temp) >= 35 ? "ON" : "OFF");
                break;
            //* Button 4 (P1.4-P1.0): Low water level alert
            case BUTTON_ALERT:
                display_common("Alert Level:",
                               water == 1 ? "Empty" : water == 50 ? "Half"
                                                  : water == 100  ? "Full"
                                                                  : "ERROR!",
                               water == 1 ? "OFF" : "");
                break;
            default:
                /* PENDING Handle LCD display restoration */
                display_common("Enter a number:", "> 1 2 3 4", "");
                break;
        }

        /* TARGET: Automatic act pump - Dry soil and water available*/
        PUMP_CONTROL = (soil <= 30 && water > 1) ? TURN_ON : TURN_OFF;

        /* TARGET: Automatic act fan - High temperature */
        FAN_CONTROL = (atoi(temp) >= 35) ? TURN_ON : TURN_OFF;

        /* DEBUG Send data to ESP32 via UART once 20s
         * Send string UART data with 4 variables "soil,temp,hum,water"
         * NOTE: Don't send too much data at once, as it may result in data loss or unexpected errors.
         */
        // Format string safely data for ESP32
        sprintf(shared_buffer, "%u,%s,", soil, temp);
        UART_Send_String(shared_buffer);
        sprintf(shared_buffer, "%s,%u", hum, water);
        UART_Send_String(shared_buffer);

        // Delay before next iteration
        delay(2000); // Short delay to keep system responsive
    }
}

unsigned int getSoilMoisture(void)
{
    /* Active-low mode
     * Function: Read soil moisture level from digital pin.
     * Returns: 100 (wet) or 0 (dry) based on SOIL_MOISTURE_PIN state.
     */
    // Read pin once to avoid noise
    unsigned int pin_state = SOIL_MOISTURE_PIN;
    if (pin_state == 0) // Low = wet
        return 100;
    else // High = dry
        return 0;
}

unsigned int getWaterLevel(void)
{
    /* Read water level using two digital pins.
     * Return: 1 -> Empty, 50 -> Half, 100 -> Full
     */
    unsigned int min_pin = WATER_LEVEL_MIN_PIN;
    unsigned int max_pin = WATER_LEVEL_MAX_PIN;
    if (min_pin == 0 && max_pin == 0) {
        return 1; // Empty
    } else if (min_pin == 1 && max_pin == 0) {
        return 50; // Half
    } else if (min_pin == 1 && max_pin == 1) {
        return 100; // Full
    }
    return 0; // Invalid state (fail-safe)
}

unsigned char scan_keypad(void)
{
    // Scan Row 4 // Active-low
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

void display_common(const char *header, const char *info, const char *status)
{
    LCD_Command(0x01);
    LCD_String_xy(0, 0, header);
    LCD_String_xy(1, 0, info);
    LCD_String_xy(1, 13, status);
    if (status == "OFF") beep(250, 2); // Beep to indicate
}
