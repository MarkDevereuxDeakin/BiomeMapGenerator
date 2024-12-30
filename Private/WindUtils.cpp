#include "WindUtils.h"

bool WindUtils::IsOnshoreWind(FVector2D WindDirection, FVector2D OceanToLandVector)
{
    float DotProduct = FVector2D::DotProduct(WindDirection.GetSafeNormal(), OceanToLandVector.GetSafeNormal());
    return DotProduct > 0.0f; // Onshore if dot product is positive
}

void WindUtils::AdjustWeatherFactors(bool IsOnshore, float WindStrength, float& Precipitation, float& Temperature, float DistanceToOcean)
{
    if (IsOnshore && DistanceToOcean < 150000.0f) // Onshore winds close to the ocean
    {
        Precipitation *= 1.0f + (0.9f * WindStrength); // Increase precipitation by wind strength
        Temperature += 1.0f * WindStrength;           // Slightly warmer due to moist air
    }
    else if (!IsOnshore && DistanceToOcean > 50000.0f) // Offshore winds far from the ocean
    {
        Precipitation *= 0.7f;                        // Decrease precipitation for offshore winds
        Temperature -= 1.5f * WindStrength;           // Cooler due to dry air
    }
}
