#pragma once

#include "CoreMinimal.h"

/**
 * Structure defining the weightings for biomes.
 */
struct FBiomeWeights
{
    float TemperatureWeight;      // Weight for temperature influence
    float PrecipitationWeight;    // Weight for precipitation influence
};

/** Global weight map associating biomes with their respective weights. */
extern TMap<FString, FBiomeWeights> BiomeWeightMap;

/**
 * Calculate biome probabilities based on environmental parameters.
 * @param AdjustedTemperature - Adjusted temperature value.
 * @param Precipitation - Total precipitation value.
 * @param Candidates - List of candidate biomes.
 * @return The most probable biome as a string.
 */
FString CalculateBiomeProbabilities(float AdjustedTemperature, float Precipitation, TArray<FString> Candidates);
