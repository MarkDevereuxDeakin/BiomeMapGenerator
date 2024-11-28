#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "PlanetTime.h"
#include "UObject/Object.h"
#include "BiomeCalculator.generated.h"

UCLASS()
class BIOMEMAPPER_API UBiomeCalculator : public UObject
{
    GENERATED_BODY()

public:
    // Main biome calculation function
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiome(
        float Latitude, 
        float Longitude, 
        float Altitude, 
        float DistanceToOcean, 
        FString FlowDirection,
        const FPlanetTime& PlanetTime);

         // New version for heightmap processing
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiomeFromInput(
        const FString& MinLatitudeStr,
        const FString& MaxLatitudeStr,
        const FString& MinLongitudeStr,
        const FString& MaxLongitudeStr,
        const FString& MinAltitudeStr,
        const FString& MaxAltitudeStr,
        const FString& SeaLevelStr,
        const TArray<FHeightmapCell>& HeightmapData,
        const FPlanetTime& PlanetTime); 

        // Declare the function to filter biome candidates
    TArray<FString> FilterBiomeCandidates(float AdjustedTemperature, float Precipitation, float OceanTempEffect);   

};