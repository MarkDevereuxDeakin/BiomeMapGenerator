#pragma once

#include "CoreMinimal.h"

class OceanTemperature
{
public:
    // Calculate the ocean temperature adjustment based on latitude, longitude, and flow direction
    static float CalculateOceanTemp(float Temperature, float DistanceToOcean, float Latitude, float Longitude, FString FlowDirection);
};
