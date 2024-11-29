#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
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
     * @param Latitude - Geographic latitude.
     * @param Longitude - Geographic longitude.
     * @param Altitude - Altitude of the location.
     * @param DistanceToOcean - Distance to the nearest ocean.
     * @param FlowDirection - Ocean or wind flow direction.
     * @param PlanetTime - The planetary time information.
     * @return The calculated biome as a string.
     */
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiome(
        float Latitude, 
        float Longitude, 
        float Altitude, 
        float DistanceToOcean, 
        FString FlowDirection,
        const FPlanetTime& PlanetTime);

    /**
     * Calculate biomes for an entire heightmap input.
     * @param MinLatitudeStr - Minimum latitude (string format).
     * @param MaxLatitudeStr - Maximum latitude (string format).
     * @param MinLongitudeStr - Minimum longitude (string format).
     * @param MaxLongitudeStr - Maximum longitude (string format).
     * @param MinAltitudeStr - Minimum altitude (string format).
     * @param MaxAltitudeStr - Maximum altitude (string format).
     * @param SeaLevelStr - Sea level (string format).
     * @param HeightmapData - The heightmap data array.
     * @param PlanetTime - The planetary time information.
     * @return The calculated biome data as a string.
     */
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

    /**
     * Filter biome candidates based on environmental parameters.
     * @param AdjustedTemperature - Temperature after adjustments.
     * @param Precipitation - Total precipitation.
     * @return Array of biome candidates.
     */
    TArray<FString> FilterBiomeCandidates(float AdjustedTemperature, float Precipitation);
};
