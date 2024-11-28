#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API WindUtils
{
public:
    /**
     * Determine if the wind is onshore or offshore.
     * @param WindDirection The calculated wind direction vector.
     * @param OceanToLandVector The vector pointing from ocean to land.
     * @return True if the wind is onshore, false if offshore.
     */
    static bool IsOnshoreWind(FVector2D WindDirection, FVector2D OceanToLandVector);

    /**
     * Adjust precipitation and temperature based on wind patterns.
     * @param IsOnshore Whether the wind is onshore.
     * @param WindStrength The magnitude of the wind vector.
     * @param Precipitation Current precipitation value (mm/year).
     * @param Temperature Current temperature value (°C).
     * @param DistanceToOcean Distance to the nearest ocean (meters).
     */
    static void AdjustWeatherFactors(bool IsOnshore, float WindStrength, float& Precipitation, float& Temperature, float DistanceToOcean);

};