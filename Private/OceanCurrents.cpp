#include "OceanCurrents.h"

FString OceanCurrents::DetermineOceanCurrentType(float Latitude, float Longitude, FString FlowDirection)
{
    // Validate and adjust the flow direction if needed
    FlowDirection = ValidateFlowDirection(Latitude, Longitude, FlowDirection);

    // Determine the ocean current type
    if (FlowDirection == "Clockwise" && Latitude < 30.0f)
    {
        return "warm"; // Warm current heading toward poles
    }
    else if (FlowDirection == "Counterclockwise" && Latitude < 30.0f)
    {
        return "warm"; // Warm current heading toward poles
    }
    else if (FlowDirection == "Counterclockwise" && Latitude > 30.0f)
    {
        return "cold"; // Cold current heading toward equator
    }
    else if (FlowDirection == "Clockwise" && Latitude > 30.0f)
    {
        return "cold"; // Cold current heading toward equator
    }

    return "cold";
}

FString OceanCurrents::ValidateFlowDirection(float Latitude, float Longitude, FString FlowDirection)
{
    // Ensure valid flow direction based on location
    if (Latitude >= 0.0f) // Northern Hemisphere
    {
        if (Longitude >= 0.0f) // Eastern Hemisphere
        {
            return "Clockwise";
        }
        else // Western Hemisphere
        {
            return "Counterclockwise";
        }
    }
    else // Southern Hemisphere
    {
        if (Longitude >= 0.0f) // Eastern Hemisphere
        {
            return "Counterclockwise";
        }
        else // Western Hemisphere
        {
            return "Clockwise";
        }
    }
}
