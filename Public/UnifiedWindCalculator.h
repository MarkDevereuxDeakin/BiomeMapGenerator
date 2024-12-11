#pragma once

#include "CoreMinimal.h"

/**
 * Unified calculator for refining wind directions by integrating multiple models.
 */
class UnifiedWindCalculator
{
public:
    /**
     * Calculate a refined wind direction based on multiple environmental factors.
     * @param Latitude - Geographic latitude (in degrees).
     * @param Longitude - Geographic longitude (in degrees).
     * @param TimeOfYear - Fractional time of the year (e.g., 0.0 for start, 1.0 for end).
     * @return Refined wind direction as a 2D vector (X, Y).
     */
    static FVector2D CalculateRefinedWind(float Latitude, float Longitude, float TimeOfYear);
};
