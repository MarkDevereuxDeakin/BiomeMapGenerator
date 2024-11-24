#include "SeasonalWinds.h"
#include "GlobalWind.h" // Include the header file for GlobalWind

FVector2D SeasonalWinds::CalculateSeasonalWindDirection(float Latitude, float TimeOfYear)
{
    if (Latitude >= -30.0f && Latitude <= 30.0f)
    {
        // Simulate monsoon-like seasonal shifts
        if (TimeOfYear < 0.5f)
        {
            // Pre-monsoon winds: Southwesterly (example)
            return FVector2D(0.7f, -0.7f);
        }
        else
        {
            // Post-monsoon winds: Northeasterly (example)
            return FVector2D(-0.7f, 0.7f);
        }
    }
    else
    {
        // Default to global wind patterns for other latitudes
        return GlobalWind::CalculateWindDirection(Latitude);
    }
}
