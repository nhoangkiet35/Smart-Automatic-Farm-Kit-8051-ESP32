#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <REGX52.H>

// Define digital pins as input
sbit WATER_LEVEL_MIN_PIN = P1 ^ 5; // Half level sensor
sbit WATER_LEVEL_MAX_PIN = P1 ^ 6; // Full level sensor

// Function prototypes
unsigned int getWaterLevel(void);

#endif
