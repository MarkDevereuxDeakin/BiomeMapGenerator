#include "BiomeWeightedProbability.h"

// Define the biome weight map
// Define BiomeWeightMap
TMap<FString, FBiomeWeights> BiomeWeightMap = {
    { "Tropical Rainforest", { 1.5f, 3.0f} },
    { "Savanna", { 2.0f, 2.5f} },
    { "Tropical Monsoon Forests", { 1.8f, 2.7f} },
    { "Temperate Steppe and Savanna", { 1.8f, 1.5f } },
    { "Temperate Broadleaf", { 2.0f, 2.0f } },
    { "Subtropical Evergreen Forest", { 2.0f, 2.0f } },
    { "Mediterranean", { 1.5f, 2.0f} },
    { "Xeric Shrubland", { 2.5f, 0.7f } },
    { "Dry Forest and Woodland Savanna", { 2.0f, 2.5f } },
    { "Hot Arid Desert", { 3.0f, 0.5f } },
    { "Tundra", { 3.0f, 0.8f } },
    { "Montane Forests and Grasslands", { 2.5f, 2.0f } },
    { "Taiga (Boreal Forest)", { 3.0f, 1.5f } },
    { "Cold or Polar Desert", { 3.5f, 0.5f } }
};

FString CalculateBiomeProbabilities(float AdjustedTemperature, float Precipitation, TArray<FString> Candidates)
{
    TArray<TPair<FString, float>> Probabilities;

    for (const FString& Candidate : Candidates)
    {        
        const FBiomeWeights* WeightsPtr = BiomeWeightMap.Find(Candidate);

        if (WeightsPtr == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Biome '%s' not found in BiomeWeightMap. Skipping."), *Candidate);
            continue;
        }

        const FBiomeWeights& Weights = *WeightsPtr;

        // Calculate the score for the biome
        float Score = Weights.TemperatureWeight * AdjustedTemperature +
                      Weights.PrecipitationWeight * Precipitation;

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
