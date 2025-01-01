#pragma once

#include "CoreMinimal.h"

/**
 * Class for calculating precipitation based on environmental parameters.
 */
class BIOMEMAPPER_API Precipitation
{
public:
    /**
     * Calculate the annual precipitation for a location.
     * @param Latitude - Geographic latitude (in degrees).
     * @param Altitude - Altitude of the location (in meters).
     * @param DistanceToOcean - Distance to the nearest ocean (in meters).
     * @param RelativeHumidity - Relative humidity as a percentage [0, 100].
     * @return Calculated precipitation in mm/year.
     */
    static float CalculatePrecipitation(
    float Latitude,
    float Altitude,
    float DistanceToOcean,
    //float RelativeHumidity,
    float Slope,
    FVector2D WindDirection,
    FVector2D OceanToLandVector);
};
