#include "BiomeWeightedProbability.h"
#include "Async/ParallelFor.h"

// Define BiomeWeightMap
TMap<FString, FBiomeWeights> BiomeWeightMap = {
    { "Tropical Rainforest", { 1.5f, 3.0f } },
    { "Savanna", { 2.0f, 2.5f } },
    { "Tropical Monsoon Forests", { 1.8f, 2.7f } },
    { "Temperate Steppe and Savanna", { 1.8f, 1.5f } },
    { "Temperate Broadleaf", { 2.0f, 2.0f } },
    { "Subtropical Evergreen Forest", { 2.0f, 2.0f } },
    { "Mediterranean", { 1.5f, 2.0f } },
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
    Probabilities.Reserve(Candidates.Num());

    // Temporary array to hold scores
    TArray<float> Scores;
    Scores.SetNumZeroed(Candidates.Num());

    // Parallel calculation of scores
    ParallelFor(Candidates.Num(), [&](int32 Index)
    {
        const FString& Candidate = Candidates[Index];
        const FBiomeWeights* WeightsPtr = BiomeWeightMap.Find(Candidate);

        if (!WeightsPtr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Biome '%s' not found in BiomeWeightMap. Skipping."), *Candidate);
            Scores[Index] = 0.0f;
            return;
        }

        const FBiomeWeights& Weights = *WeightsPtr;

        // Calculate the score for the biome
         Scores[Index] = FMath::Max(0.0f, Weights.TemperatureWeight * AdjustedTemperature +
                                              Weights.PrecipitationWeight * Precipitation);
    });

    float TotalScore = 0.0f;

    for (float Score : Scores)
    {
        TotalScore += Score;
    }
    
    FString BestBiome = "Unknown Biome";
    float MaxProbability = 0.0f;

    for (int32 Index = 0; Index < Candidates.Num(); ++Index)
    {
        if (Scores[Index] > 0.0f)
        {
            float Probability = Scores[Index] / TotalScore;
            if (Probability > MaxProbability)
            {
                MaxProbability = Probability;
                BestBiome = Candidates[Index];
            }
        }
    }

    return BestBiome;
}
