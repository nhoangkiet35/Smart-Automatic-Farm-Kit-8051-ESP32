#include "WaterLevel.h"

unsigned int getWaterLevel(void)
{
    /*
     * Read water level using two digital pins.
     * Return:
     * 1   -> Empty
     * 50  -> Half
     * 100 -> Full
     */
    if (WATER_LEVEL_MIN_PIN == 0 && WATER_LEVEL_MAX_PIN == 0)
        return 1; // Empty
    else if (WATER_LEVEL_MIN_PIN == 1 && WATER_LEVEL_MAX_PIN == 0)
        return 50; // Half
    else if (WATER_LEVEL_MIN_PIN == 1 && WATER_LEVEL_MAX_PIN == 1)
        return 100; // Full
    else
        return 0; // Invalid state (fail-safe)
}
