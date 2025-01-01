#pragma once

#include "CoreMinimal.h"
#include "PlanetTime.h"
#include "Humidity.h"

/**
 * Class for calculating surface temperature based on environmental factors.
 */
class BIOMEMAPPER_API Temperature
{
public:
    /**
     * Calculate the surface temperature for a given location.
     * @param Latitude - Geographic latitude (in degrees).
     * @param Altitude - Altitude of the location (in meters).
     * @param DayOfYear - Day of the year (1-based).
     * @param Humidity - Relative humidity as a percentage [0, 100].
     * @param PlanetTime - Planetary time information.
     * @return Calculated surface temperature in Celsius.
     */
    static float CalculateSurfaceTemperature(
        float Latitude,
        float Altitude,
        int DayOfYear,
        //float Humidity,
        const FPlanetTime& PlanetTime,
        float Slope,
        float Aspect,
        float WindSpeed);

};
