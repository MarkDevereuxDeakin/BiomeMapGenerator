#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API GlobalWind
{
public:
    static FVector2D CalculateWindDirection(float Latitude);
};
