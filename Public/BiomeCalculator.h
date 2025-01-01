#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "BiomeInputShared.h"
#include "PlanetTime.h"
#include "UObject/Object.h"
#include "BiomeCalculator.generated.h"

/**
 * Class responsible for calculating biome classifications.
 */
UCLASS()
class BIOMEMAPPER_API UBiomeCalculator : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Calculate the biome for specific coordinates.
     * @param Cell - The Heightmap Cell
     * @param PlanetTime - The planetary time information.
     * @return The calculated biome as a string.
     */
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiome(FHeightmapCell& Cell);

    /**
     * Calculate biomes for an entire heightmap input.
     * @param InputParams - Struct containing the input variables
     * @param MinLongitude - Calculated minimum longitude (float).
     * @param MaxLongitude - Calculated maximum longitude (float).     * 
     * @param HeightmapData - The heightmap data array.
     * @param PlanetTime - The planetary time information.
     * @return The calculated biome data as a string.
     */
    
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiomeFromInput(
        FInputParameters& InputParams,             
        float MinLongitude, // Use calculated Min Longitude
        float MaxLongitude, // Use calculated Max Longitude        
        TArray<FHeightmapCell>& HeightmapData);

    /**
     * Filter biome candidates based on environmental parameters.
     * @param Temperature - Temperature after adjustments.
     * @param AnnualPrecipitation - Total precipitation.
     * @return Array of biome candidates.
     */
    UFUNCTION(BlueprintCallable, category = "Biome Calculator")
    TArray<FString> FilterBiomeCandidates(float Temperature, float AnnualPrecipitation, float Latitude, float Altitude/*, float Humidity*/);

};
