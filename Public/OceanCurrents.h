#pragma once

#include "CoreMinimal.h"

class OceanCurrents
{
public:
    /**
     * Determines the type of ocean current based on latitude, longitude, and flow direction.
     * 
     * @param Latitude The latitude in degrees (-90 to 90).
     * @param Longitude The longitude in degrees (-180 to 180).
     * @param FlowDirection The general flow direction of the current (Clockwise or Counterclockwise).
     * @return The type of ocean current ("warm" or "cold").
     */
    static FString DetermineOceanCurrentType(float Latitude, float Longitude, FString FlowDirection);

private:
    // Utility function to validate the inputs and prevent "unknown" outputs.
    static FString ValidateFlowDirection(float Latitude, float Longitude, FString FlowDirection);
};
