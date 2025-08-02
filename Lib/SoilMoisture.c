#include "SoilMoisture.h"

unsigned int getSoilMoisture(void)
{
    /*
     * Active-low mode
     * Function: Read soil moisture level from digital pin.
     * Returns: 100 (wet) or 0 (dry) based on SOIL_MOISTURE_PIN state.
     */
    if (SOIL_MOISTURE_PIN == 0) // Low = wet
        return 100;
    else // High = dry
        return 0;
}
