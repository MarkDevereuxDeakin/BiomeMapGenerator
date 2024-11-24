#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API SeasonalWinds
{
public:
    static FVector2D CalculateSeasonalWindDirection(float Latitude, float TimeOfYear);
};
