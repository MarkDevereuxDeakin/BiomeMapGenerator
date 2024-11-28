#include "Precipitation.h"
#include "Math/UnrealMathUtility.h"

float Precipitation::CalculatePrecipitation(float Latitude, float Altitude, float DistanceToOcean, float RelativeHumidity)
{
    // Latitude-based precipitation (scaled to reflect wet tropics and drier poles)
    float latitudeFactor = 2000.0f * FMath::Clamp(FMath::Cos(FMath::DegreesToRadians(Latitude)), 0.0f, 1.0f);

    // Ocean proximity effect: Decreases precipitation with distance from ocean
    float oceanFactor = FMath::Max(200.0f - (DistanceToOcean * 0.002f), -100.0f); // Adjusted for meters

    // Altitude effect: Increases up to 2 km, then decreases
    float altitudeFactor;
    if (Altitude < 2000.0f)
    {
        altitudeFactor = 0.2f * Altitude; // Increase up to 2000m
    }
    else
    {
        altitudeFactor = -5.0f * (Altitude - 2000.0f) / 1000.0f; // Decrease beyond 2000m
    }

    // Humidity scaling: Higher humidity amplifies precipitation
    float humidityFactor = FMath::Clamp(RelativeHumidity / 100.0f, 0.0f, 2.0f); // Normalize to [0, 2]

    // Combine all factors
    float precipitation = (latitudeFactor + oceanFactor + altitudeFactor) * humidityFactor;

    // Ensure non-negative precipitation
    return FMath::Max(precipitation, 0.0f);
}
