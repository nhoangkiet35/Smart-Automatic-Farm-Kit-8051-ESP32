#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include <REGX52.H>

// Define digital pin
sbit SOIL_MOISTURE_PIN = P2 ^ 4; // Digital pin D0

// Function prototypes
void SoilMoisture_Init(void);
unsigned int getSoilMoisture(void);

#endif