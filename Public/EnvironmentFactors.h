#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API EnvironmentFactors
{
public:
    static float CalculateTemperature(float Latitude, float Altitude);
    static float CalculatePrecipitation(float Latitude, float Altitude, float ProximityToOcean);
};
