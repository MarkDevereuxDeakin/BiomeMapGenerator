#pragma once

#include "CoreMinimal.h"
#include "PlanetTime.h"
#include "Humidity.h"

class BIOMEMAPPER_API Temperature
{
public:
    static float CalculateSurfaceTemperature(float Latitude, float Altitude, int DayOfYear, float Humidity, const FPlanetTime& PlanetTime);
    static void TestLatitudeTemperature();
};
