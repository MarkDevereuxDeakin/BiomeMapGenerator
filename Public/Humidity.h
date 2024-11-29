#pragma once

#include "CoreMinimal.h"

/**
 * Class for calculating humidity levels based on environmental factors.
 */
class BIOMEMAPPER_API Humidity
{
public:
    /**
     * Calculate the base humidity range based on latitude and distance to the ocean.
     * @param Latitude - Geographic latitude.
     * @param DistanceToOcean - Distance to the nearest ocean (in meters).
     * @return Base humidity as a percentage.
     */
    static float GetBaseHumidity(float Latitude, float DistanceToOcean);

    /**
     * Calculate the relative humidity for a region.
     * @param Latitude - Geographic latitude.
     * @param DistanceToOcean - Distance to the nearest ocean (in meters).
     * @param IsOnshore - True if wind is onshore, false otherwise.
     * @return Relative humidity as a percentage.
     */
    static float CalculateRelativeHumidity(float Latitude, float DistanceToOcean, bool IsOnshore);
};
