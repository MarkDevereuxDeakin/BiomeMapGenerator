#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API Albedo
{
public:

    // Calculate base albedo based on latitude
    static float CalculateAlbedo(float Latitude);

    // Adjust albedo for snow and ice
    static float AdjustAlbedoForSnow(float Albedo, float Temperature);

    // Adjust albedo based on precipitation and vegetation cover
    static float AdjustAlbedoForPrecipitation(float Albedo, float Precipitation);

    // Compute dynamic albedo based on environmental factors
    static void CalculateDynamicAlbedo(TArray<FHeightmapCell>& HeightmapData);
};