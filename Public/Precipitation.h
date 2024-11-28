#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API Precipitation
{
public:
    /**
     * Calculate precipitation based on latitude, altitude, ocean proximity, and humidity.
     * @param Latitude The latitude in degrees.
     * @param Altitude The altitude in meters.
     * @param DistanceToOCean Distance to the nearest ocean in meters.
     * @param RelativeHumidity Relative humidity as a percentage [0, 100].
     * @return The calculated precipitation in mm/year.
     */
    static float CalculatePrecipitation(float Latitude, float Altitude, float DistanceToOcean, float RelativeHumidity);
};
