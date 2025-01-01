#include "Precipitation.h"
#include "WindUtils.h"
#include "SlopeAndAspect.h"
#include "Math/UnrealMathUtility.h"

float Precipitation::CalculatePrecipitation(
    float Latitude,
    float Altitude,
    float DistanceToOcean,
    float Slope,
    FVector2D WindDirection,
    FVector2D OceanToLandVector)
{
    // Latitude-based precipitation (scaled to reflect wet tropics and drier poles)
    float LatitudeFactor = 2000.0f * FMath::Clamp(FMath::Cos(FMath::DegreesToRadians(Latitude)), 0.0f, 1.0f);

    // Ocean proximity effect: Decreases precipitation with distance from ocean
   float OceanFactor = 200.0f * FMath::Exp(-DistanceToOcean / 50000.0f); // Adjusted for meters

    // Altitude effect: Increases up to 2 km, then decreases    
    float AltitudeFactor = (Altitude < 2000.0f) ? 0.2f * Altitude : -5.0f * (Altitude - 2000.0f) / 1000.0f;

    // Orographic effect
    float OrographicEffect = (Slope > 5.0f && WindUtils::IsOnshoreWind(WindDirection, OceanToLandVector)) ? FMath::Pow(Slope, 1.2f) * 15.0f : 0.0f;

    // Combine all factors
    float Precipitation = (LatitudeFactor + OceanFactor + AltitudeFactor);

    // Ensure non-negative precipitation
    return FMath::Max(Precipitation, 0.0f);
}
