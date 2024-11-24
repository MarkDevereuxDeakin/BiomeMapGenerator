#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API PressureBasedWind
{
public:
    static FVector2D CalculatePressureBasedWind(float Latitude, float Longitude);
};
