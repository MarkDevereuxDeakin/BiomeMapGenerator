#pragma once

#include "CoreMinimal.h"


class BIOMEMAPPER_API Humidity
{
public:
    /**
     * Get the base humidity range based on latitude and region classification.
     * @param Latitude The latitude of the location.
     * @return The base humidity as a percentage.
     */
    static float GetBaseHumidity(float Latitude, float DistanceToOcean);

    /**
     * Calculate relative humidity for a pixel/region.
     * @param Latitude The latitude of the location.
     * @param DistanceToOcean The distance to the nearest ocean (meters).
     * @param IsOnshore Whether the wind is onshore (true) or offshore (false).
     * @return The calculated relative humidity as a percentage.
     */
    static float CalculateRelativeHumidity(float Latitude, float DistanceToOcean, bool IsOnshore);
};
