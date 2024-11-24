#include "OceanTemperature.h"
#include "OceanCurrents.h" // Include the header file for OceanCurrents

const float OCEAN_TEMP_EFFECT_WARM = 5.0f;
const float OCEAN_TEMP_EFFECT_COLD = -5.0f;

float OceanTemperature::CalculateOceanTemp(float Temperature, float DistanceToOcean, float Latitude, float Longitude, FString FlowDirection)
{
    // Use the OceanCurrents module to determine the current type
    FString CurrentType = OceanCurrents::DetermineOceanCurrentType(Latitude, Longitude, FlowDirection);

    // Adjust temperature based on the determined current type
    if (CurrentType == "warm")
    {
        Temperature += OCEAN_TEMP_EFFECT_WARM / ((DistanceToOcean/1000.0f) + 1);
    }
    else if (CurrentType == "cold")
    {
        Temperature += OCEAN_TEMP_EFFECT_COLD / ((DistanceToOcean/1000.0f) + 1);
    }

    return Temperature;
}
