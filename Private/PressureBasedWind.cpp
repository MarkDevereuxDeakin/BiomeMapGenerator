#include "PressureBasedWind.h"

FVector2D PressureBasedWind::CalculatePressureBasedWind(float Latitude, float Longitude)
{
    // Example pressure gradient calculation (simplified for demonstration)
    float PressureGradient = -FMath::Sin(FMath::DegreesToRadians(Latitude));

    if (PressureGradient > 0.0f)
    {
        // High to low pressure movement (clockwise in the Northern Hemisphere)
        return FVector2D(1.0f, -PressureGradient);
    }
    else
    {
        // High to low pressure movement (counterclockwise in the Southern Hemisphere)
        return FVector2D(-1.0f, PressureGradient);
    }
}
