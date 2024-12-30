#include "Albedo.h"
#include "HeightmapCell.h"
#include "Async/ParallelFor.h"
#include <algorithm>

// Calculate base Albedo based on Latitude
float Albedo::CalculateAlbedo(float Latitude) {
    if (Latitude < 30.0f) return 0.15f; // Low Latitude (tropics)
    if (Latitude < 60.0f) return 0.25f; // Mid Latitude (temperate)
    return 0.60f; // High Latitude (polar)
}

// Adjust Albedo for snow and ice
float Albedo::AdjustAlbedoForSnow(float Albedo, float Temperature) {
    if (Temperature < 0.0f) return Albedo + 0.2f; // Increase Albedo for snow/ice
    return Albedo; // No snow/ice adjustment in warmer regions
}

// Adjust Albedo based on Precipitation and vegetation cover
float Albedo::AdjustAlbedoForPrecipitation(float Albedo, float Precipitation) {
    if (Precipitation > 1000.0f) return Albedo - 0.1f; // Dense vegetation reduces Albedo
    if (Precipitation < 250.0f) return Albedo + 0.1f; // Sparse vegetation increases Albedo
    return Albedo;
}

// Compute dynamic Albedo based on environmental factors
void Albedo::CalculateDynamicAlbedo(TArray<FHeightmapCell>& HeightmapData)
{
     // Precompute OceanToLandVectors
    ParallelFor(HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];
        
        if (Cell.DistanceToOcean > 0.0f)
        {
            // Step 1: Calculate base Albedo based on Latitude.
            Cell.Albedo = CalculateAlbedo(Cell.Latitude);

            // Step 2: Adjust Albedo for Snow and Temperature below freezing
            if(Cell.Temperature < 0.0f)
            {
                Cell.Albedo = AdjustAlbedoForSnow(Cell.Albedo, Cell.Temperature);
            }
            else
            {
                // Step 3: Adjust Albedo for Vegetation, ie Annual Rainfall
                Cell.Albedo = AdjustAlbedoForPrecipitation(Cell.Albedo, Cell.AnnualPrecipitation);
            }
        }

        // Ensure that the Albedo stays within realistic bounds
        Cell.Albedo = FMath::Clamp(Cell.Albedo, 0.05f, 0.80f);
    });   
   
}