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
     * @param MinLatitudeSlider - Minimum latitude (Slider).
     * @param MaxLatitudeSlider - Maximum latitude (Slider).
     * @param OutMinLongitude - Calculated minimum longitude (float).
     * @param OutMaxLongitude - Calculated maximum longitude (float).
     * @param MinAltitudeStr - Minimum altitude (string format).
     * @param MaxAltitudeStr - Maximum altitude (string format).
     * @param SeaLevelStr - Sea level (string format).
     * @param HeightmapData - The heightmap data array.
     * @param PlanetTime - The planetary time information.
     * @return The calculated biome data as a string.
     */
    
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    FString CalculateBiomeFromInput(
        float MinLatitude, // Use value from slider
        float MaxLatitude, // Use value from slider
        float OutMinLongitude, // Use calculated Min Longitude
        float OutMaxLongitude, // Use calculated Max Longitude
        float MinAltitudeStr, // Use value from slider
        float MaxAltitudeStr, // Use value from slider
        float SeaLevelStr, // Use value from slider
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
