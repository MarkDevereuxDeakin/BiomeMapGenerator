#include "Temperature.h"
#include "PlanetTime.h"
#include "Math/UnrealMathUtility.h"

const float TEMP_BASE_EQUATOR = 30.0f;
const float TEMP_DROP_PER_KM_ALTITUDE = 6.5f;
const float TEMP_DROP_PER_M_ALTITUDE = TEMP_DROP_PER_KM_ALTITUDE / 1000.0f; // Update to meters

const float TROPICAL_LATITUDE_THRESHOLD = 23.5f;
const float TEMPERATE_LATITUDE_THRESHOLD = 66.5f;
const float POLAR_LATITUDE_THRESHOLD = 90.0f;

float Temperature::CalculateSurfaceTemperature(float Latitude, float Altitude, int DayOfYear, float Humidity, const FPlanetTime& PlanetTime)
{
    // Solar declination angle based on day of year
    float DeclinationAngle = 23.5f * FMath::Cos(2.0f * PI * (DayOfYear / PlanetTime.GetYearLength()));
    float SolarInsolation = FMath::Max(0.0f, FMath::Cos(FMath::DegreesToRadians(Latitude - DeclinationAngle)));

    // Base temperature at sea level
    float SurfaceTemp = TEMP_BASE_EQUATOR * SolarInsolation;

    // Adjust lapse rate based on humidity (lower lapse rate for higher humidity)
    float LapseRate = (Humidity > 50.0f) ? 4.0f / 1000.0f : 6.5f / 1000.0f; // °C per meter

    // Adjust for altitude
    if (Altitude < 11000.0f) // Troposphere
    {
        SurfaceTemp -= (LapseRate * Altitude);
    }
    else // Stratosphere
    {
        float StratosphereRate = 3.0f / 1000.0f; // Warming rate in stratosphere
        SurfaceTemp -= (LapseRate * 11000.0f) + (StratosphereRate * (Altitude - 11000.0f));
    }

    // Adjust for nighttime cooling (higher humidity reduces cooling)
    float NighttimeCooling = (1.0f - Humidity / 100.0f) * 5.0f; // Example cooling factor
    SurfaceTemp -= NighttimeCooling;

    return SurfaceTemp;
}