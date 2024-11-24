#include "EnvironmentFactors.h"
#include "Math/UnrealMathUtility.h"

const float TEMP_BASE_EQUATOR = 30.0f;
const float TEMP_DROP_PER_KM_ALTITUDE = 6.5f;
const float TEMP_DROP_PER_M_ALTITUDE = TEMP_DROP_PER_KM_ALTITUDE / 1000.0f; // Update to meters

const float TROPICAL_LATITUDE_THRESHOLD = 23.5f;
const float TEMPERATE_LATITUDE_THRESHOLD = 66.5f;
const float POLAR_LATITUDE_THRESHOLD = 90.0f;

float EnvironmentFactors::CalculateTemperature(float Latitude, float Altitude)
{
    float angleRatio = std::cos(FMath::DegreesToRadians(Latitude));
    float surfaceTemp = TEMP_BASE_EQUATOR * angleRatio;
    float tempAtAltitude = surfaceTemp - (TEMP_DROP_PER_M_ALTITUDE * Altitude);
    return tempAtAltitude;
}

float EnvironmentFactors::CalculatePrecipitation(float Latitude, float Altitude, float ProximityToOcean)
{
    // Base precipitation as a continuous function of latitude
    float latitudeFactor = 2000.0f * FMath::Clamp(std::cos(FMath::DegreesToRadians(Latitude)), 0.0f, 1.0f);

    // Proximity to ocean decreases precipitation as a function of distance
    float oceanFactor = FMath::Max(200.0f - (ProximityToOcean * 0.002f), -100.0f);// Adjusted for Metres

    // Altitude: Precipitation increases up to 2 km, then decreases
    float altitudeFactor;
    if (Altitude < 2000.0f) {
        altitudeFactor = 0.2f * Altitude; // Increase up to 2000m
    } else {
        altitudeFactor = -5.0f * (Altitude - 2000.0f)/1000.0f; // Decrease beyond 2000m
    }

    // Combine factors, ensure non-negative precipitation
    float precipitation = latitudeFactor + oceanFactor + altitudeFactor;
    return FMath::Max(precipitation, 0.0f);
}
