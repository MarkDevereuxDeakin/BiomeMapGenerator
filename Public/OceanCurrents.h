#pragma once

#include "CoreMinimal.h"

/**
 * Class for determining ocean currents and their properties.
 */
class OceanCurrents
{
public:
    /**
     * Determine the type of ocean current (warm or cold) based on location and flow direction.
     * @param Latitude - Geographic latitude (-90 to 90 degrees).
     * @param Longitude - Geographic longitude (-180 to 180 degrees).
     * @param FlowDirection - General flow direction ("Clockwise" or "Counterclockwise").
     * @return The type of ocean current ("warm" or "cold").
     */
    static FString DetermineOceanCurrentType(float Latitude, float Longitude, FString FlowDirection);

private:
    /**
     * Validate input parameters for current determination.
     * @param Latitude - Geographic latitude.
     * @param Longitude - Geographic longitude.
     * @param FlowDirection - General flow direction.
     * @return Corrected or validated flow direction.
     */
    static FString ValidateFlowDirection(float Latitude, float Longitude, FString FlowDirection);
};
