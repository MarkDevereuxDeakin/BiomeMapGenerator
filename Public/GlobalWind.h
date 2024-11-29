#pragma once

#include "CoreMinimal.h"

/**
 * Class to calculate global wind directions based on latitude.
 */
class BIOMEMAPPER_API GlobalWind
{
public:
    /**
     * Calculate wind direction at a given latitude.
     * @param Latitude - Geographic latitude.
     * @return Wind direction as a 2D vector.
     */
    static FVector2D CalculateWindDirection(float Latitude);
};
