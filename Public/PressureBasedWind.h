#pragma once

#include "CoreMinimal.h"

/**
 * Class for calculating wind direction based on atmospheric pressure gradients.
 */
class BIOMEMAPPER_API PressureBasedWind
{
public:
    /**
     * Calculate wind direction based on pressure gradients.
     * @param Latitude - Geographic latitude (in degrees).
     * @param Longitude - Geographic longitude (in degrees).
     * @return Wind direction as a 2D vector (X, Y).
     */
    static FVector2D CalculatePressureBasedWind(float Latitude, float Longitude);
};
