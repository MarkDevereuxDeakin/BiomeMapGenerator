#include "OceanTemperature.h"
#include "OceanCurrents.h" // Include the header file for OceanCurrents

float OceanTemperature::CalculateOceanTemp(float Temperature, float DistanceToOcean, float Latitude, float Longitude, FString FlowDirection)
{
    // Determine the ocean current type (warm or cold)
    FString CurrentType = OceanCurrents::DetermineOceanCurrentType(Latitude, Longitude, FlowDirection);

    // Define WaterEffect based on the current type
    float WaterEffect = (CurrentType == "warm") ? 5.0f : -5.0f; // Replace constants with WaterEffect

    // Apply the temperature adjustment based on WaterEffect and DistanceToOcean
    Temperature += WaterEffect / ((DistanceToOcean / 1000.0f) + 1);

    return Temperature;
}
