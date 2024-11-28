#include "BiomeMapper.h"
#include "PlanetTime.h"
#include "WindUtils.h"
#include "Temperature.h"
#include "Precipitation.h"
#include "OceanTemperature.h"
#include "UnifiedWindCalculator.h"
#include "BiomeEditorMode.h"
#include "EditorModeRegistry.h"
#include "Editor/EditorEngine.h" // Ensure this is included for editor functionality

#define LOCTEXT_NAMESPACE "FBiomeMapperModule"

void FBiomeMapperModule::StartupModule()
{
    // Register the Biome Editor Mode
    FEditorModeRegistry::Get().RegisterMode<FBiomeEditorMode>(
        FBiomeEditorMode::EM_BiomeEditorMode,
        LOCTEXT("BiomeEditorModeName", "Biome Mapper"),
        FSlateIcon(), // Use a default or replace with your own icon
        true
    );
   

    FString FilePath = FPaths::ProjectSavedDir() / "LatitudeTemperatureLog.txt";

    FString Output = "Latitude, Temperature, Humidity\n";

    float Latitude;

    FPlanetTime PlanetTime(86400.0f, 365.25f, 12);

    for (float i = 0.0f; i <= 90.0f; i += 1.0f)
        {
            Latitude = i;

            // Get day of year from planetary time
        int32 DayOfYear = PlanetTime.GetDayOfYear();       

        // Calculate wind direction based on latitude, longitude, day of the year and time of day.
        FVector2D WindDirection = UnifiedWindCalculator::CalculateRefinedWind(
            Latitude, 
            1, 
            PlanetTime.GetTimeOfDay() / PlanetTime.GetDayLengthSeconds());

        // Calculate magnitude of wind
        float WindStrength = WindDirection.Size();

        // Define the ocean-to-land vector (example assumes ocean westward)
        FVector2D OceanToLandVector = FVector2D(1.0f, 0.0f);

        //Determine if the wind is onshore
        bool IsOnshore = WindUtils::IsOnshoreWind(WindDirection, OceanToLandVector);

        // Calculate relative humidity
        float RelativeHumidity = Humidity::CalculateRelativeHumidity(Latitude, 1, IsOnshore);

        // Calculate precipitation
        float Precipitation = Precipitation::CalculatePrecipitation(Latitude, 1, 1, RelativeHumidity);

        // Calculate base temperature
        float BaseTemperature = Temperature::CalculateSurfaceTemperature(Latitude, 1, PlanetTime.GetDayOfYear(), RelativeHumidity, PlanetTime); 
        
        // Adjust temperature based on ocean effects
        float AdjustedTemperature = OceanTemperature::CalculateOceanTemp(BaseTemperature, 1, Latitude, 1, "clockwise");
        float OceanTempEffect = AdjustedTemperature / (1 + 1);

        //float OceanTempEffect = AdjustedTemperature * FMath::Exp(-DistanceToOcean / 100.0f); Alternative OCeanTempEffect, with diminitioning effect as distance increases

        WindUtils::AdjustWeatherFactors(IsOnshore, WindStrength, Precipitation, AdjustedTemperature, 1);

         Output += FString::Printf(TEXT("%.1f, %.2f, %.3f\n"), Latitude, AdjustedTemperature, RelativeHumidity);
        }
        // Save to file
    FFileHelper::SaveStringToFile(Output, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());

    // Optional: Log to Unreal console
    UE_LOG(LogTemp, Log, TEXT("Latitude temperature test completed. Results saved to: %s"), *FilePath);

    UE_LOG(LogTemp, Log, TEXT("BiomeMapper Module started successfully!"));
}

void FBiomeMapperModule::ShutdownModule()
{
    // Unregister the Biome Editor Mode
    FEditorModeRegistry::Get().UnregisterMode(FBiomeEditorMode::EM_BiomeEditorMode);

    UE_LOG(LogTemp, Log, TEXT("BiomeMapper Module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBiomeMapperModule, BiomeMapper)
