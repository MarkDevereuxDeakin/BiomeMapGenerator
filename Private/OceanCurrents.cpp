#include "OceanCurrents.h"

#include "OceanCurrents.h"

FString OceanCurrents::DetermineOceanCurrentType(float Latitude, float Longitude, FString FlowDirection)
{
    // Validate and adjust the flow direction
    FlowDirection = ValidateFlowDirection(Latitude, Longitude, FlowDirection);

    // Determine the ocean current type based on latitude and flow direction
    if (Latitude >= 0.0f) // Northern Hemisphere
    {
        if (FlowDirection == "Clockwise")
        {
            return (Latitude < 40.0f) ? "warm" : "cold";
        }
        else
        {
            return "cold";
        }
    }
    else // Southern Hemisphere
    {
        if (FlowDirection == "Counterclockwise")
        {
            return (Latitude > -40.0f) ? "warm" : "cold";
        }
        else
        {
            return "cold";
        }
    }
}

FString OceanCurrents::ValidateFlowDirection(float Latitude, float Longitude, FString FlowDirection)
{
    // Determine gyre direction based on latitude and longitude
    if (Latitude >= 0.0f) // Northern Hemisphere
    {
        return (Longitude >= 0.0f) ? "Clockwise" : "Counterclockwise";
    }
    else // Southern Hemisphere
    {
        return (Longitude >= 0.0f) ? "Counterclockwise" : "Clockwise";
    }
}

