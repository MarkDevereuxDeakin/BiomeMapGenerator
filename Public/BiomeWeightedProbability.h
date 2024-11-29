#pragma once

#include "CoreMinimal.h"

// Structure to define weightings for each biome
struct FBiomeWeights
{
    float TemperatureWeight;
    float PrecipitationWeight;
};

// Declare the weight map
extern TMap<FString, FBiomeWeights> BiomeWeightMap;

// Function to calculate probabilities for biomes based on environmental parameters
FString CalculateBiomeProbabilities(float AdjustedTemperature, float Precipitation, TArray<FString> Candidates);
