#include "UnifiedWindCalculator.h"
#include "GlobalWind.h"
#include "PressureBasedWind.h"
#include "SeasonalWinds.h"

FVector2D UnifiedWindCalculator::CalculateRefinedWind(float Latitude, float Longitude, float TimeOfYear)
{
    // Base global wind pattern
    FVector2D GlobalWindDirection = GlobalWind::CalculateWindDirection(Latitude);

    // Pressure-based adjustment
    FVector2D PressureWind = PressureBasedWind::CalculatePressureBasedWind(Latitude, Longitude);

    // Seasonal variation adjustment
    FVector2D SeasonalWind = SeasonalWinds::CalculateSeasonalWindDirection(Latitude, TimeOfYear);

    // Combine wind vectors with weighted contributions
    FVector2D CombinedWind = GlobalWindDirection * 0.5f + PressureWind * 0.3f + SeasonalWind * 0.2f;

    // Normalize to return a unit vector
    return CombinedWind.GetSafeNormal();
}
