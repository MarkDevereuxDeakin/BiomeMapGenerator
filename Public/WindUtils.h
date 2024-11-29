#pragma once

#include "CoreMinimal.h"

/**
 * Utility class for calculating and adjusting wind-related factors.
 */
class BIOMEMAPPER_API WindUtils
{
public:
    /**
     * Determine if the wind is onshore or offshore based on direction.
     * @param WindDirection - The calculated wind direction vector (X, Y).
     * @param OceanToLandVector - The vector pointing from ocean to land (X, Y).
     * @return True if the wind is onshore, false if offshore.
     */
    static bool IsOnshoreWind(FVector2D WindDirection, FVector2D OceanToLandVector);

    /**
     * Adjust precipitation and temperature based on wind patterns and proximity to the ocean.
     * @param IsOnshore - Whether the wind is onshore (true) or offshore (false).
     * @param WindStrength - The magnitude of the wind vector.
     * @param Precipitation - Current precipitation value (mm/year). Modified in place.
     * @param Temperature - Current temperature value (°C). Modified in place.
     * @param DistanceToOcean - Distance to the nearest ocean (in meters).
     */
    static void AdjustWeatherFactors(bool IsOnshore, float WindStrength, float& Precipitation, float& Temperature, float DistanceToOcean);
};