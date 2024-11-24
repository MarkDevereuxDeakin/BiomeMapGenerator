#include "BiomeCalculator.h"
#include "OceanTemperature.h"
#include "EnvironmentFactors.h"
#include "UnifiedWindCalculator.h"

// Helper function to classify biome
static FString DetermineBiome(float AdjustedTemperature, float Precipitation, float OceanTempEffect);

FString UBiomeCalculator::CalculateBiome(float Latitude, float Longitude, float Altitude, float DistanceToOcean, FString FlowDirection)
{
    // Calculate base temperature and precipitation
    float BaseTemperature = EnvironmentFactors::CalculateTemperature(Latitude, Altitude);
    float Precipitation = EnvironmentFactors::CalculatePrecipitation(Latitude, Altitude, DistanceToOcean);

    // Adjust temperature based on ocean effects
    float AdjustedTemperature = OceanTemperature::CalculateOceanTemp(BaseTemperature, DistanceToOcean, Latitude, Longitude, FlowDirection);
    float OceanTempEffect = AdjustedTemperature / (DistanceToOcean + 1);

    //float OceanTempEffect = AdjustedTemperature * FMath::Exp(-DistanceToOcean / 100.0f); Alternative OCeanTempEffect, with diminitioning effect as distance increases


    // Adjust precipitation and temperature based on wind patterns
    float TimeOfYear = 0.5f; // Placeholder for mid-year; update dynamically if needed
    FVector2D WindDirection = UnifiedWindCalculator::CalculateRefinedWind(Latitude, Longitude, TimeOfYear);

    if (WindDirection.X > 0 && DistanceToOcean < 50.0f) // Onshore winds
    {
        Precipitation *= 1.2f;
        AdjustedTemperature += 1.5f;
    }
    else if (WindDirection.X < 0 && DistanceToOcean > 50.0f) // Offshore winds
    {
        Precipitation *= 0.8f;
        AdjustedTemperature -= 1.5f;
    }

    // Determine biome based on adjusted values
    return DetermineBiome(AdjustedTemperature, Precipitation, OceanTempEffect);
}

FString UBiomeCalculator::CalculateBiomeFromInput(
    const FString& MinLatitudeStr, 
    const FString& MaxLatitudeStr, 
    const FString& MinLongitudeStr, 
    const FString& MaxLongitudeStr, 
    const FString& MinAltitudeStr, 
    const FString& MaxAltitudeStr, 
    const FString& SeaLevelStr,
    const TArray<FHeightmapCell>& HeightmapData)
{
    // Convert string inputs to floats
    float MinLatitude = FCString::Atof(*MinLatitudeStr);
    float MaxLatitude = FCString::Atof(*MaxLatitudeStr);
    float MinLongitude = FCString::Atof(*MinLongitudeStr);
    float MaxLongitude = FCString::Atof(*MaxLongitudeStr);
    float MinAltitude = FCString::Atof(*MinAltitudeStr) / 1000.0f;
    float MaxAltitude = FCString::Atof(*MaxAltitudeStr) / 1000.0f;
    float SeaLevel = FCString::Atof(*SeaLevelStr) / 1000.0f;

    // Check for invalid input ranges
    if (MinLatitude > MaxLatitude || MinLongitude > MaxLongitude || MinAltitude > MaxAltitude)
    {
        return "Invalid input ranges provided.";
    }

   // Shared string for final results (use mutex for thread-safe access)
    FString FinalBiomes;
    FCriticalSection ResultMutex;

    // Parallel loop over heightmap cells
    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        const FHeightmapCell& Cell = HeightmapData[Index];

        // Ensure the cell falls within the specified bounds
        if (Cell.Latitude < MinLatitude || Cell.Latitude > MaxLatitude ||
            Cell.Longitude < MinLongitude || Cell.Longitude > MaxLongitude ||
            Cell.Altitude < MinAltitude || Cell.Altitude > MaxAltitude)
        {
            return; // Skip this cell
        }

        // Calculate biome
        FString Biome = CalculateBiome(Cell.Latitude, Cell.Longitude, Cell.Altitude, Cell.DistanceToOcean, "");

        // Append results (thread-safe)
        {
            FScopeLock Lock(&ResultMutex);
            FinalBiomes += FString::Printf(TEXT("(%f, %f): %s\n"), Cell.Latitude, Cell.Longitude, *Biome);
        }

        
    });

        // Check if FinalBiomes is empty
    if (FinalBiomes.IsEmpty())
    {
        return "No valid data found in the provided heightmap.";
    }

    // Return the accumulated results
    return FinalBiomes;
}


static FString DetermineBiome(float AdjustedTemperature, float Precipitation, float OceanTempEffect)
{
    // Handle extreme climates
    if (AdjustedTemperature > 30 || Precipitation > 3000)
        return "Extreme Tropical Climate";
    if (AdjustedTemperature < -20 || Precipitation < 50)
        return "Extreme Polar Climate";

    // Biome classification logic
    if (AdjustedTemperature > 26 && Precipitation > 2500) return "Tropical Rainforest";
    if (AdjustedTemperature > 24 && Precipitation > 1000 && Precipitation <= 2500) return "Savanna";
    if (AdjustedTemperature > 18 && AdjustedTemperature <= 24 && Precipitation > 800 && Precipitation <= 2000) return "Tropical Seasonal Forest";
    if (AdjustedTemperature > 12 && AdjustedTemperature <= 18 && Precipitation > 1000 && OceanTempEffect > 800) return "Temperate Broadleaf";
    if (AdjustedTemperature > 5 && AdjustedTemperature <= 15 && Precipitation <= 600 && OceanTempEffect > 500) return "Temperate Steppe and Savanna";
    if (AdjustedTemperature > 15 && AdjustedTemperature <= 18 && Precipitation > 1000 && OceanTempEffect > 1200) return "Subtropical Evergreen Forest";
    if (AdjustedTemperature > 15 && AdjustedTemperature <= 20 && Precipitation >= 500 && Precipitation <= 1000 && OceanTempEffect > 1000) return "Mediterranean";
    if (AdjustedTemperature > 18 && Precipitation >= 1500 && Precipitation <= 2500 && OceanTempEffect > 800) return "Monsoon Forests";
    if (AdjustedTemperature > 20 && Precipitation < 200 && OceanTempEffect < 500) return "Arid Desert";
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 20 && Precipitation >= 250 && Precipitation <= 500 && OceanTempEffect < 700) return "Xeric Shrubland";
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 20 && Precipitation >= 200 && Precipitation <= 400 && OceanTempEffect < 600) return "Dry Steppe";
    if (AdjustedTemperature >= 5 && AdjustedTemperature <= 18 && Precipitation >= 400 && Precipitation <= 800 && OceanTempEffect < 500) return "Semiarid Desert";
    if (AdjustedTemperature >= 18 && AdjustedTemperature <= 22 && Precipitation >= 500 && Precipitation <= 1000 && OceanTempEffect > 600) return "Grass Savanna";
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 26 && Precipitation >= 800 && Precipitation <= 1500 && OceanTempEffect > 800) return "Tree Savanna";
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 28 && Precipitation >= 1000 && Precipitation <= 2000 && OceanTempEffect > 900) return "Dry Forest and Woodland Savanna";
    if (AdjustedTemperature >= -10 && AdjustedTemperature <= -5 && Precipitation >= 300 && Precipitation <= 600 && OceanTempEffect > 500) return "Alpine Tundra";
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 10 && Precipitation > 1000 && OceanTempEffect > 700) return "Montane Forests and Grasslands";
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 5 && Precipitation >= 400 && Precipitation <= 1500 && OceanTempEffect > 500 && OceanTempEffect <= 1000) return "Taiga (Boreal Forest)";
    if (AdjustedTemperature >= 15 && Precipitation <= 500 && OceanTempEffect < 500) return "Scrub/Shrubland";
    if (AdjustedTemperature < 0 && Precipitation <= 300 && OceanTempEffect <= 300) return "Tundra";
    if (AdjustedTemperature < -5 && Precipitation < 100 && OceanTempEffect < 200) return "Polar Desert";
    if (AdjustedTemperature > 20 && Precipitation < 250 && OceanTempEffect < 500) return "Hot Desert";
    if (AdjustedTemperature < 10 && Precipitation < 250 && OceanTempEffect < 500) return "Cold Desert";

    // Fallback case
    return FString::Printf(TEXT("Unknown Biome (Temp: %.2f, Precip: %.2f, OceanTempEffect: %.2f)"),
                           AdjustedTemperature, Precipitation, OceanTempEffect);
}
