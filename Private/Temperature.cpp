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
    float DeclinationAngle = 23.5f * FMath::Cos(2.0f * PI * (DayOfYear / PlanetTime.GetYearLengthDays()));
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

void Temperature::TestLatitudeTemperature()
{
    TArray<float> Latitudes = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f };
    float Altitude = 0.0f; // Use sea level for testing
    FString FilePath = FPaths::ProjectSavedDir() / "LatitudeTemperatureLog.txt";

    FString Output = "Latitude, Temperature\n";
    FPlanetTime PlanetTime(86400.0f, 365.25f, 12);

    for (float Latitude : Latitudes)
    {
        float Temperature = Temperature::CalculateSurfaceTemperature(Latitude, Altitude, 1, 0.5f, PlanetTime);
        Output += FString::Printf(TEXT("%.1f, %.2f\n"), Latitude, Temperature);
    }

    // Save to file
    FFileHelper::SaveStringToFile(Output, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());

    // Optional: Log to Unreal console
    UE_LOG(LogTemp, Log, TEXT("Latitude temperature test completed. Results saved to: %s"), *FilePath);
}