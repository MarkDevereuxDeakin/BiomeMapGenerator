#include "Humidity.h"
#include "Math/UnrealMathUtility.h"

float Humidity::GetBaseHumidity(float Latitude, float DistanceToOcean)
{
    // Convert Southern Hemisphere latitude to its Northern Hemisphere equivalent
    float AbsoluteLatitude = FMath::Abs(Latitude);

    // Calculate base humidity with distance-based adjustment
    float BaseHumidity;
    if (AbsoluteLatitude < 25.0f) // Tropical
    {
        BaseHumidity = FMath::Clamp(90.0f - (DistanceToOcean / 5000.0f), 75.0f, 90.0f);
    }
    else if (AbsoluteLatitude < 40.0f) // Temperate
    {
        BaseHumidity = FMath::Clamp(70.0f - (DistanceToOcean / 10000.0f), 50.0f, 70.0f);
    }
    else if (AbsoluteLatitude < 66.5f) // Sub-Arctic/Grasslands
    {
        BaseHumidity = FMath::Clamp(60.0f - (DistanceToOcean / 15000.0f), 40.0f, 60.0f);
    }
    else // Arctic/Tundra
    {
        BaseHumidity = FMath::Clamp(50.0f - (DistanceToOcean / 20000.0f), 20.0f, 50.0f);
    }

    // Add random variation (±5%)
    float RandomVariation = FMath::FRandRange(-5.0f, 5.0f);
    BaseHumidity += RandomVariation;

    // Ensure the result is within valid bounds
    return FMath::Clamp(BaseHumidity, 0.0f, 100.0f);
}

float Humidity::CalculateRelativeHumidity(float Latitude, float DistanceToOcean, float Altitude, bool IsOnshore)
{
    // Step 1: Get base humidity
    float BaseHumidity = GetBaseHumidity(Latitude, DistanceToOcean);

    // Apply exponential distance decay
    BaseHumidity *= exp(-DistanceToOcean/500000.0);

    // Adjust for Altitude
    BaseHumidity -= 0.05 * Altitude;

    // Step 2: Adjust for wind direction
    if (IsOnshore)
    {
        float WindEffect = FMath::FRandRange(5.0f, 15.0f); // Randomize wind effect
        BaseHumidity += FMath::Clamp(WindEffect - (DistanceToOcean / 100000.0f), 1.0f, 15.0f);
    }
    else
    {
        float WindEffect = FMath::FRandRange(5.0f, 20.0f); // Randomize offshore wind effect
        BaseHumidity -= FMath::Clamp(DistanceToOcean /1000000.0f + WindEffect, 1.0f, 20.0f);
    }

    // Add random perturbation (±3%)
    float RandomPerturbation = FMath::FRandRange(-3.0f, 3.0f);
    BaseHumidity += RandomPerturbation;

    BaseHumidity = FMath::Max(BaseHumidity, 20.0f); // Ensure minimum for land cells

    // Ensure humidity stays within realistic bounds
    return FMath::Clamp(BaseHumidity, 0.0f, 100.0f);
}
