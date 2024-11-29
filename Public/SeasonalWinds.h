#pragma once

#include "CoreMinimal.h"

/**
 * Class for calculating seasonal wind patterns.
 */
class BIOMEMAPPER_API SeasonalWinds
{
public:
    /**
     * Calculate the wind direction based on latitude and time of year.
     * @param Latitude - Geographic latitude (in degrees).
     * @param TimeOfYear - Time of the year (e.g., day of year or fractional year).
     * @return Seasonal wind direction as a 2D vector (X, Y).
     */
    static FVector2D CalculateSeasonalWindDirection(float Latitude, float TimeOfYear);
};
