#pragma once

#include "CoreMinimal.h"

/**
 * Structure defining the weightings for biomes.
 */
struct FBiomeWeights
{
    float TempWeight;       // Weight for temperature influence
    float PrecWeight;       // Weight for precipitation influence    
    float LatitudeWeight;   // Weight for latitude influence
    float AltitudeWeight;   // Weight for altitude influence
    float SlopeWeight;      // Weight for slope influence
    float AspectWeight;     // Weight for aspect influence
};

/** Global weight map associating biomes with their respective weights. */
extern TMap<FString, FBiomeWeights> BiomeWeightMap;

/**
 * Calculate biome probabilities based on environmental parameters.
 * @param AdjustedTemperature - Adjusted temperature value.
 * @param Precipitation - Total precipitation value.
 * @param Latitude - Latitude value.
 * @param Altitude - Altitude value.
 * @param Slope - Slope value.
 * @param Aspect - Aspect value.
 * @param Candidates - List of candidate biomes.
 * @return The most probable biome as a string.
 */
FString CalculateBiomeProbabilities(
    float AdjustedTemperature,
    float Precipitation,
    float Latitude,
    float Altitude,
    float Slope,
    float Aspect,
    TArray<FString> Candidates);
