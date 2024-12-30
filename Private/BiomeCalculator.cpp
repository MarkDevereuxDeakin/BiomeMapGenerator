#include "BiomeCalculator.h"
#include "BiomeInputShared.h"
#include "Async/ParallelFor.h"
#include "BiomeWeightedProbability.h"
#include "LoggingUtils.h"

// Map of biome colors
static const TMap<FString, FColor> BiomeColorMap = {
    {"Tropical Rainforest", FColor::FromHex("#006400")},
    {"Tropical Monsoon Forests", FColor::FromHex("#228B22")},
    {"Savanna", FColor::FromHex("#F5DEB3")},
    {"Temperate Steppe and Savanna", FColor::FromHex("#8b7d63")},
    {"Temperate Broadleaf", FColor::FromHex("#877116")},
    {"Subtropical Evergreen Forest", FColor::FromHex("#89b855")},
    {"Mediterranean", FColor::FromHex("#513058")},
    {"Xeric Shrubland", FColor::FromHex("#c4947c")},
    {"Dry Forest and Woodland Savanna", FColor::FromHex("#59463a")},
    {"Hot Arid Desert", FColor::FromHex("#782713")},
    {"Tundra", FColor::FromHex("#30abf8")},
    {"Montane Forests and Grasslands", FColor::FromHex("#077891")},
    {"Taiga and Boreal Forests", FColor::FromHex("#084f45")},
    {"Cold or Polar Desert", FColor::FromHex("#D3D3D3")}
};

// Helper function to classify biome
//static TArray<FString> FilterBiomeCandidates(float AdjustedTemperature, float Precipitation, float OceanTempEffect);
int counter = 100;
FString UBiomeCalculator::CalculateBiome(FHeightmapCell& Cell)
{
    // Filter Biomes based on adjusted values
    TArray<FString> Candidates = FilterBiomeCandidates(Cell.Temperature, Cell.AnnualPrecipitation, Cell.Latitude, Cell.Altitude, Cell.RelativeHumidity);

    // Directly select the first candidate without weighted probabilities
    FString Biome = Candidates.Num() > 0 ? Candidates[0] : "Unknown Biome";

    // Assign biome type and color to the cell
    if (!Biome.IsEmpty() && BiomeColorMap.Contains(Biome))
    {
        Cell.BiomeType = Biome;
        Cell.BiomeColor = BiomeColorMap[Biome];
    }
    else
    {
        Cell.BiomeType = "Unknown";
        Cell.BiomeColor = FColor::Black;
    }

    return Biome;
    /*
    // Filter Biomes based on adjusted values
    TArray<FString> Candidates = FilterBiomeCandidates(Cell.Temperature, Cell.AnnualPrecipitation, Cell.Latitude, Cell.Altitude, Cell.RelativeHumidity);

    // Determine the best biome
    FString Biome = Candidates.Num() > 0 && Candidates[0] != "Unknown Biome"
                        ? CalculateBiomeProbabilities(Cell.Temperature, Cell.AnnualPrecipitation, Cell.RelativeHumidity, Cell.Latitude, Cell.Altitude, Cell.Slope, Cell.Aspect, Candidates)
                        : "Unknown Biome";

    // Assign biome type and color to the cell
    if (!Biome.IsEmpty() && BiomeColorMap.Contains(Biome))
    {
        Cell.BiomeType = Biome;
        Cell.BiomeColor = BiomeColorMap[Biome];
    }
    else
    {
        Cell.BiomeType = "Unknown";
        Cell.BiomeColor = FColor::Black;
    }

    return Biome;*/
}

FString UBiomeCalculator::CalculateBiomeFromInput(
    FInputParameters& InputParams,        
    float MinLongitude, // Use calculated Min Longitude
    float MaxLongitude, // Use calculated Max Longitude    
    TArray<FHeightmapCell>& HeightmapData)
{
    //FInputParameters InputParams = GetInputParameters();
    
    // Check for invalid input ranges
    if (InputParams.SouthernLatitude > InputParams.NorthernLatitude || MinLongitude > MaxLongitude || InputParams.MinimumAltitude > InputParams.MaximumAltitude)
    {
        return "Invalid input ranges provided.";
    }

    TMap<FString, int32> UniqueBiomes;
    FCriticalSection ResultMutex;

    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        FHeightmapCell& Cell = HeightmapData[Index];

        if (Cell.CellType == ECellType::Land &&
            Cell.Latitude >= InputParams.SouthernLatitude && Cell.Latitude <= InputParams.NorthernLatitude &&
            Cell.Longitude >= MinLongitude && Cell.Longitude <= MaxLongitude &&
            Cell.Altitude >= InputParams.MinimumAltitude && Cell.Altitude <= InputParams.MaximumAltitude)
        {
            FString Biome = CalculateBiome(Cell);

            if(!Biome.IsEmpty())
            {
               FScopeLock Lock(&ResultMutex); 
               UniqueBiomes.FindOrAdd(Biome)++;
            }            
        }
    });

    // Log data to CSV after processing
    FString LogFilePath = FPaths::ProjectDir() + TEXT("BiomeDataLog.csv");
    LogBiomeDataToCSV(HeightmapData, LogFilePath);

    if (UniqueBiomes.Num() == 0)
    {
        return "No valid data found in the provided heightmap.";
    }

    FString FinalBiomes = "Detected Biomes \n";
    for (const TPair<FString, int32>& Pair : UniqueBiomes)
    {
        FinalBiomes += FString::Printf(TEXT("%s: %d occurrences\n"), *Pair.Key, Pair.Value);
    }

    return FinalBiomes;
}

TArray<FString> UBiomeCalculator::FilterBiomeCandidates(float AdjustedTemperature, float Precipitation, float Latitude, float Altitude, float Humidity)
{
    // Array of candidate biomes
    TArray<FString> Candidates;

   // Biome classification logic
   // Apply latitude modifiers for temperature thresholds
    bool IsTropical = (Latitude >= -23.5f && Latitude <= 23.5f);
    bool IsTemperate = (Latitude > 23.5f && Latitude <= 60.0f) || (Latitude < -23.5f && Latitude >= -60.0f);
    bool IsPolar = (Latitude > 60.0f || Latitude < -60.0f);

    if (AdjustedTemperature >= 24.0f && Precipitation >= 2000.0f && IsTropical) Candidates.Add("Tropical Rainforest");
    if (AdjustedTemperature >= 24.0f && Precipitation >= 1500.0f && Precipitation <= 2000.0f && IsTropical) Candidates.Add( "Tropical Monsoon Forests");
    if (AdjustedTemperature >= 20.0f && Precipitation >= 500.0f && Precipitation <= 1500.0f && Humidity > 50.0f && IsTropical) Candidates.Add( "Savanna"); //More tropical. Hot wet summer and cooler dry winters    
    if (AdjustedTemperature >= 10.0f && AdjustedTemperature <= 20.0f && Precipitation >= 250.0f && Precipitation <= 750.0f && IsTemperate) Candidates.Add( "Temperate Steppe and Savanna");// Semiarid transition between grassland and desert. Short grasses, hot summers and cold winters
    if (AdjustedTemperature >= 10.0f && AdjustedTemperature <= 20.0f && Precipitation >= 750.0f && Precipitation <= 1500.0f && IsTemperate ) Candidates.Add( "Temperate Broadleaf");    
    if (AdjustedTemperature >= 10.0f && Precipitation >= 1500.0f && Humidity > 60.0f && IsTemperate ) Candidates.Add( "Subtropical Evergreen Forest");
    if (AdjustedTemperature > 10.0f && AdjustedTemperature <= 40.0f && Precipitation >= 250.0f && Precipitation <= 900.0f && IsTemperate) Candidates.Add( "Mediterranean");
    if (AdjustedTemperature >= 10.0f && Precipitation <= 250.0f  && Humidity < 30.0f) Candidates.Add( "Xeric Shrubland");// similar to mediterranean but drier. Xeric actually means dry
    if (AdjustedTemperature >= 0.0f && Precipitation >= 250.0f && Precipitation <= 500.0f && !IsPolar ) Candidates.Add( "Dry Forest and Woodland Savanna");   
    if (AdjustedTemperature > 25.0f && Precipitation < 250.0f && Humidity < 20.0f) Candidates.Add( "Hot Arid Desert");
    if (AdjustedTemperature < 12.0f && Altitude >= 2000.0f ) Candidates.Add( "Tundra");// Polar Tundra    
    if (AdjustedTemperature >= 0.0f && AdjustedTemperature <= 15.0f && Precipitation > 1000.0f && Altitude > 1500.0f ) Candidates.Add( "Montane Forests and Grasslands");
    if (AdjustedTemperature <= 15.0f && Precipitation >= 500.0f && Precipitation <= 1500.0f && IsPolar ) Candidates.Add( "Taiga (Boreal Forest)");       
    if (AdjustedTemperature < 5.0f && Precipitation < 250.0f && IsPolar) Candidates.Add("Cold or Polar Desert");

    // Add a fallback if no candidates were found - remove or comment this out if debugging is complete
    
    if (Candidates.Num() == 0 && counter > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No biomes matched for Temp=%.2f, Precip=%.2f"),
               AdjustedTemperature, Precipitation);
        Candidates.Add("Unknown Biome");

        counter--;
    }

    return Candidates;
}

/*
FInputParameters UBiomeCalculator::GetInputParameters() const
{
    return InputParams;
}*/

/**
 * 
 *  // Biome classification logic
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 35 && Precipitation >= 2000) Candidates.Add("Tropical Rainforest");
    if (AdjustedTemperature >= 15 && AdjustedTemperature <= 35 && Precipitation >= 1000) Candidates.Add( "Tropical Monsoon Forests");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 30 && Precipitation >= 500 && Precipitation <= 1500) Candidates.Add( "Savanna"); //More tropical. Hot wet summer and cooler dry winters    
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 25 && Precipitation >= 250 && Precipitation <= 750 ) Candidates.Add( "Temperate Steppe and Savanna");// Semiarid transition between grassland and desert. Short grasses, hot summers and cold winters
    if (AdjustedTemperature >= -30 && AdjustedTemperature <= 30 && Precipitation >= 750 && Precipitation <= 2000 ) Candidates.Add( "Temperate Broadleaf");    
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 26 && Precipitation >= 1000 && Precipitation < 2500 ) Candidates.Add( "Subtropical Evergreen Forest");
    if (AdjustedTemperature > 10 && AdjustedTemperature <= 40 && Precipitation >= 400 && Precipitation <= 900 ) Candidates.Add( "Mediterranean");
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 40 && Precipitation >= 100 && Precipitation <= 400 ) Candidates.Add( "Xeric Shrubland");// similar to mediterranean but drier. Xeric actually means dry
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 40 && Precipitation >= 400 && Precipitation <= 1250 ) Candidates.Add( "Dry Forest and Woodland Savanna");   
    if (AdjustedTemperature > 30 && Precipitation < 250 ) Candidates.Add( "Hot Arid Desert");
    if (AdjustedTemperature > -34 && AdjustedTemperature < 12 && Precipitation <= 250 ) Candidates.Add( "Tundra");// Polar Tundra    
    if (AdjustedTemperature >= 0 && AdjustedTemperature <= 15 && Precipitation > 1000 && Precipitation <= 2200 ) Candidates.Add( "Montane Forests and Grasslands");
    if (AdjustedTemperature >= -54 && AdjustedTemperature <= 15 && Precipitation >= 250 && Precipitation <= 750 ) Candidates.Add( "Taiga (Boreal Forest)");       
    if (AdjustedTemperature < 5 && Precipitation < 250) Candidates.Add("Cold or Polar Desert");
 */

/**
 * // Old Biome classification logic
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 25 && Precipitation >= 2000) Candidates.Add("Tropical Rainforest");
    if (AdjustedTemperature >= 24 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1650) Candidates.Add( "Savanna");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 26 && Precipitation >= 1000 && Precipitation <= 2000) Candidates.Add( "Tropical Seasonal Forest");
    if (AdjustedTemperature >= -30 && AdjustedTemperature <= 30 && Precipitation >= 750 && Precipitation <= 1700 > 800) Candidates.Add( "Temperate Broadleaf");
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 20 && Precipitation >= 250 && Precipitation <= 500 > 500) Candidates.Add( "Temperate Steppe and Savanna");
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 26 && Precipitation >= 1000 && Precipitation < 2000 > 1200) Candidates.Add( "Subtropical Evergreen Forest");
    if (AdjustedTemperature > 10 && AdjustedTemperature <= 35 && Precipitation >= 500 && Precipitation <= 900 > 1000) Candidates.Add( "Mediterranean");
    if (AdjustedTemperature > 19 && AdjustedTemperature < 35 && Precipitation >= 1500 && Precipitation <= 2500 > 800) Candidates.Add( "Monsoon Forests");
    if (AdjustedTemperature > 30 && AdjustedTemperature < 50 && Precipitation < 250 < 500) Candidates.Add( "Arid Desert");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 40 && Precipitation >= 200 && Precipitation <= 400 < 700) Candidates.Add( "Xeric Shrubland");
    if (AdjustedTemperature >= -5 && AdjustedTemperature <= 20 && Precipitation >= 250 && Precipitation <= 500 < 600) Candidates.Add( "Dry Steppe");
    if (AdjustedTemperature >= 10 && AdjustedTemperature <= 35 && Precipitation >= 250 && Precipitation <= 500 < 500) Candidates.Add( "Semiarid Desert");
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1500 > 600) Candidates.Add( "Grass Savanna");
    if (AdjustedTemperature >= 22 && AdjustedTemperature <= 29 && Precipitation >= 500 && Precipitation <= 1700 > 800) Candidates.Add( "Tree Savanna");
    if (AdjustedTemperature >= 20 && AdjustedTemperature <= 30 && Precipitation >= 500 && Precipitation <= 1700 > 900) Candidates.Add( "Dry Forest and Woodland Savanna");
    if (AdjustedTemperature >= -12 && AdjustedTemperature <= 10 && Precipitation >= 300 && Precipitation <= 600 > 500) Candidates.Add( "Alpine Tundra");
    if (AdjustedTemperature >= 5 && AdjustedTemperature <= 15 && Precipitation > 1000 && Precipitation <= 1500 > 700) Candidates.Add( "Montane Forests and Grasslands");
    if (AdjustedTemperature >= -54 && AdjustedTemperature <= 15 && Precipitation >= 200 && Precipitation <= 600 > 500 <= 1000) Candidates.Add( "Taiga (Boreal Forest)");
    if (AdjustedTemperature >= -1 && AdjustedTemperature <= 38 && Precipitation >= 200 && Precipitation <= 1000 < 500) Candidates.Add( "Scrub/Shrubland");
    if (AdjustedTemperature > -34 && AdjustedTemperature < 12 && Precipitation <= 250 <= 300) Candidates.Add( "Tundra");
    if (AdjustedTemperature < -10 && Precipitation < 250 < 200) Candidates.Add("Polar Desert");
    if (AdjustedTemperature > 30 && Precipitation < 250 < 500) Candidates.Add("Hot Desert");
    if (AdjustedTemperature > -5 && AdjustedTemperature < 5 && Precipitation < 250 < 500) Candidates.Add("Cold Desert");
    if (AdjustedTemperature > -5 && AdjustedTemperature < 5 && Precipitation < 250 < 500) Candidates.Add("Cold Desert");
 */