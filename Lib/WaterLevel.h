#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <REGX52.H>

// Define digital pins as input
sbit WATER_LEVEL_MIN_PIN = P1 ^ 2; // Half level sensor
sbit WATER_LEVEL_MAX_PIN = P1 ^ 3; // Full level sensor

// Function prototypes
void WaterLevel_Init(void);
void getWaterInfo(char *lcd_buffer);
unsigned int getWaterLevel(void);

#endif
