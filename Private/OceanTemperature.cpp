#include "OceanTemperature.h"
#include "OceanCurrents.h" // Include the header file for OceanCurrents

float OceanTemperature::CalculateOceanTemp(float Temperature, float DistanceToOcean, float Latitude, float Longitude, FString FlowDirection)
{
    // Determine the ocean current type (warm or cold)
    FString CurrentType = OceanCurrents::DetermineOceanCurrentType(Latitude, Longitude, FlowDirection);

    // Define WaterEffect based on the current type
    //float WaterEffect = GetWaterEffect(CurrentType, IsSummer); for when seasons are implemented
    //float WaterEffect = (CurrentType == "warm") ? 7.50f : -7.5f; // Simplistic approach

    float BaseOceanTemperature = FMath::Clamp(30.0f - FMath::Abs(Latitude) * 0.5f, -2.0f, 30.0f);
    float WaterEffect = (CurrentType == "warm") ? BaseOceanTemperature + 5.0f : BaseOceanTemperature - 5.0f;

    // Apply the temperature adjustment based on WaterEffect and DistanceToOcean
    Temperature += WaterEffect / ((DistanceToOcean / 1000.0f) + 1);

    return Temperature;
}
