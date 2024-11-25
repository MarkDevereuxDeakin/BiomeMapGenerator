#include "BiomeCalculator.h"
#include "OceanTemperature.h"
#include "EnvironmentFactors.h"
#include "UnifiedWindCalculator.h"
#include "BiomeWeightedProbability.h"

// Helper function to classify biome
//static TArray<FString> FilterBiomeCandidates(float AdjustedTemperature, float Precipitation, float OceanTempEffect);

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

    // Filter Biomes based on adjusted values
    TArray<FString> Candidates = FilterBiomeCandidates(AdjustedTemperature, Precipitation, OceanTempEffect);

    // Calculate biome probabilities
    return CalculateBiomeProbabilities(AdjustedTemperature, Precipitation, OceanTempEffect, Candidates);

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

    TSet<FString> UniqueBiomes;
    FCriticalSection ResultMutex;

    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        const FHeightmapCell& Cell = HeightmapData[Index];

        if (Cell.Latitude >= MinLatitude && Cell.Latitude <= MaxLatitude &&
            Cell.Longitude >= MinLongitude && Cell.Longitude <= MaxLongitude &&
            Cell.Altitude >= MinAltitude && Cell.Altitude <= MaxAltitude)
        {
            FString Biome = CalculateBiome(Cell.Latitude, Cell.Longitude, Cell.Altitude, Cell.DistanceToOcean, "");
            FScopeLock Lock(&ResultMutex);
            UniqueBiomes.Add(Biome);
        }
    });

    if (UniqueBiomes.Num() == 0)
    {
        return "No valid data found in the provided heightmap.";
    }

    FString FinalBiomes;
    for (const FString& Biome : UniqueBiomes)
    {
        FinalBiomes += Biome + TEXT("\n");
    }

    return FinalBiomes;
}

TArray<FString> UBiomeCalculator::FilterBiomeCandidates(float AdjustedTemperature, float Precipitation, float OceanTempEffect)
{
    // Array of candidate biomes
    TArray<FString> Candidates;

    // Handle extreme climates
    if (AdjustedTemperature > 30 || Precipitation > 3000)
        Candidates.Add( "Extreme Tropical Climate");
    if (AdjustedTemperature < -20 || Precipitation < 50)
        Candidates.Add( "Extreme Polar Climate");

    // Biome classification logic
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 25 && Precipitation >= 2000) Candidates.Add("Tropical Rainforest");
    if (AdjustedTemperature > 24 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1500) Candidates.Add( "Savanna");
    if (AdjustedTemperature >= 20 && AdjustedTemperature < 25 && Precipitation >= 1000 && Precipitation <= 2000) Candidates.Add( "Tropical Seasonal Forest");
    if (AdjustedTemperature > -30 && AdjustedTemperature <= 30 && Precipitation > 750 && Precipitation < 1500 && OceanTempEffect > 800) Candidates.Add( "Temperate Broadleaf");
    if (AdjustedTemperature > -5 && AdjustedTemperature < 20 && Precipitation >= 250 && Precipitation <= 500 && OceanTempEffect > 500) Candidates.Add( "Temperate Steppe and Savanna");
    if (AdjustedTemperature > 10 && AdjustedTemperature <= 25 && Precipitation > 1000 && Precipitation < 2000 && OceanTempEffect > 1200) Candidates.Add( "Subtropical Evergreen Forest");
    if (AdjustedTemperature > 10 && AdjustedTemperature <= 35 && Precipitation >= 500 && Precipitation <= 900 && OceanTempEffect > 1000) Candidates.Add( "Mediterranean");
    if (AdjustedTemperature > 19 && AdjustedTemperature < 35 && Precipitation >= 1500 && Precipitation <= 2500 && OceanTempEffect > 800) Candidates.Add( "Monsoon Forests");
    if (AdjustedTemperature > 30 && AdjustedTemperature < 50 && Precipitation < 250 && OceanTempEffect < 500) Candidates.Add( "Arid Desert");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 40 && Precipitation >= 200 && Precipitation <= 400 && OceanTempEffect < 700) Candidates.Add( "Xeric Shrubland");
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 20 && Precipitation >= 250 && Precipitation <= 500 && OceanTempEffect < 600) Candidates.Add( "Dry Steppe");
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 35 && Precipitation >= 250 && Precipitation <= 500 && OceanTempEffect < 500) Candidates.Add( "Semiarid Desert");
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1500 && OceanTempEffect > 600) Candidates.Add( "Grass Savanna");
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1500 && OceanTempEffect > 800) Candidates.Add( "Tree Savanna");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 30 && Precipitation >= 500 && Precipitation <= 1500 && OceanTempEffect > 900) Candidates.Add( "Dry Forest and Woodland Savanna");
    if (AdjustedTemperature >= -12 && AdjustedTemperature <= 10 && Precipitation >= 300 && Precipitation <= 600 && OceanTempEffect > 500) Candidates.Add( "Alpine Tundra");
    if (AdjustedTemperature >= 5 && AdjustedTemperature <= 15 && Precipitation > 1000 && Precipitation <= 1500 && OceanTempEffect > 700) Candidates.Add( "Montane Forests and Grasslands");
    if (AdjustedTemperature >= -54 && AdjustedTemperature <= 15 && Precipitation >= 200 && Precipitation <= 600 && OceanTempEffect > 500 && OceanTempEffect <= 1000) Candidates.Add( "Taiga (Boreal Forest)");
    if (AdjustedTemperature >= -1 && AdjustedTemperature <= 38 && Precipitation >= 200 && Precipitation <= 1000 && OceanTempEffect < 500) Candidates.Add( "Scrub/Shrubland");
    if (AdjustedTemperature > -34 && AdjustedTemperature < 12 && Precipitation <= 250 && OceanTempEffect <= 300) Candidates.Add( "Tundra");
    if (AdjustedTemperature < -10 && Precipitation < 250 && OceanTempEffect < 200) Candidates.Add("Polar Desert");
    if (AdjustedTemperature > 30 && Precipitation < 250 && OceanTempEffect < 500) Candidates.Add("Hot Desert");
    if (AdjustedTemperature > -5 && AdjustedTemperature < 5 && Precipitation < 250 && OceanTempEffect < 500) Candidates.Add("Cold Desert");

    return Candidates;
}