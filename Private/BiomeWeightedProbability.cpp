#include "BiomeWeightedProbability.h"

// Define the biome weight map
TMap<FString, FBiomeWeights> BiomeWeightMap = {
    { "Tropical Rainforest", { 1.5f, 3.0f, 0.5f } },
    { "Savanna", { 2.0f, 2.5f, 0.5f } },
    { "Tropical Seasonal Forest", { 1.8f, 2.7f, 0.5f } },
    { "Temperate Broadleaf", { 2.0f, 2.0f, 0.3f } },
    { "Temperate Steppe and Savanna", { 1.8f, 1.5f, 0.5f } },
    { "Subtropical Evergreen Forest", { 2.0f, 2.0f, 0.5f } },
    { "Mediterranean", { 1.5f, 2.0f, 0.8f } },
    { "Monsoon Forests", { 2.5f, 3.0f, 0.5f } },
    { "Arid Desert", { 3.0f, 0.5f, 0.1f } },
    { "Xeric Shrubland", { 2.5f, 0.7f, 0.1f } },
    { "Dry Steppe", { 2.0f, 1.0f, 0.1f } },
    { "Semiarid Desert", { 2.5f, 0.8f, 0.2f } },
    { "Grass Savanna", { 2.0f, 2.0f, 0.5f } },
    { "Tree Savanna", { 2.2f, 2.3f, 0.5f } },
    { "Dry Forest and Woodland Savanna", { 2.0f, 2.5f, 0.5f } },
    { "Alpine Tundra", { 3.5f, 0.8f, 0.0f } },
    { "Montane Forests and Grasslands", { 2.5f, 2.0f, 0.5f } },
    { "Taiga (Boreal Forest)", { 3.0f, 1.5f, 0.0f } },
    { "Scrub/Shrubland", { 2.5f, 1.0f, 0.3f } },
    { "Tundra", { 3.0f, 0.8f, 0.0f } },
    { "Polar Desert", { 4.0f, 0.5f, 0.0f } },
    { "Hot Desert", { 3.5f, 0.3f, 0.1f } },
    { "Cold Desert", { 3.0f, 0.5f, 0.0f } }
};

FString CalculateBiomeProbabilities(float AdjustedTemperature, float Precipitation, float OceanTempEffect, TArray<FString> Candidates)
{
    TArray<TPair<FString, float>> Probabilities;

    for (const FString& Candidate : Candidates)
    {        
        const FBiomeWeights& Weights = BiomeWeightMap[Candidate];

        // Calculate the score for the biome
        float Score = Weights.TemperatureWeight * AdjustedTemperature +
                      Weights.PrecipitationWeight * Precipitation +
                      Weights.OceanEffectWeight * OceanTempEffect;

        // Only consider biomes with positive scores
        if (Score > 0.0f)
        {
            Probabilities.Add(TPair<FString, float>(Candidate, Score));
        }
    }

    // Normalize probabilities
    float TotalScore = 0.0f;
    for (const TPair<FString, float>& Pair : Probabilities)
    {
        TotalScore += Pair.Value;
    }

    FString BestBiome = "Unknown Biome";
    float MaxProbability = 0.0f;

    for (TPair<FString, float>& Pair : Probabilities)
    {
        Pair.Value /= TotalScore; // Normalize
        if (Pair.Value > MaxProbability)
        {
            MaxProbability = Pair.Value;
            BestBiome = Pair.Key;
        }
    }

    return BestBiome;
}
