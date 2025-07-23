#include "SoilMoisture.h"

// Initialize Soil Moisture Sensor
void SoilMoisture_Init(void)
{
    // Optionally set port direction if needed (8051 ports are quasi-bidirectional by default)
    // Doing nothing here is fine
}

unsigned int getSoilMoisture(void)
{
    /*
     * Function: Read soil moisture level from digital pin.
     * Returns: 100 (wet) or 0 (dry) based on SOIL_MOISTURE_PIN state.
     */
    if (SOIL_MOISTURE_PIN == 0) // Low = wet
        return 100;
    else // High = dry
        return 0;
}

unsigned char readSoilSensor(void)
{
    return SOIL_MOISTURE_PIN; // return 0 or 1
}