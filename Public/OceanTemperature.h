#pragma once

#include "CoreMinimal.h"

/**
 * Class for calculating the ocean temperature based on environmental parameters.
 */
class OceanTemperature
{
public:
    /**
     * Calculate the ocean temperature adjustment.
     * @param Temperature - Base temperature in Celsius.
     * @param DistanceToOcean - Distance to the nearest ocean (in meters).
     * @param Latitude - Geographic latitude.
     * @param Longitude - Geographic longitude.
     * @param FlowDirection - Ocean flow direction.
     * @return Adjusted ocean temperature.
     */
    static float CalculateOceanTemp(float Temperature, float DistanceToOcean, float Latitude, float Longitude, FString FlowDirection);
};
