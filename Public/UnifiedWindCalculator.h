#pragma once

#include "CoreMinimal.h"

class UnifiedWindCalculator
{
public:
    // Calculates a refined wind direction based on all models
    static FVector2D CalculateRefinedWind(float Latitude, float Longitude, float TimeOfYear);
};
