#include "BiomeWeightedProbability.h"
#include "Async/ParallelFor.h"

// Define BiomeWeightMap
TMap<FString, FBiomeWeights> BiomeWeightMap = {
    { "Tropical Rainforest", { 3.0f, 3.5f, 2.5f, 3.0f, 0.5f, 0.0f, 0.0f } },
    { "Savanna", { 2.5f, 2.5f, 1.5f, 2.0f, 1.0f, 0.5f, 0.0f } },
    { "Tropical Monsoon Forests", { 3.0f, 3.2f, 2.0f, 3.0f, 0.5f, 0.0f, 0.0f } },
    { "Temperate Steppe and Savanna", { 1.8f, 1.5f, 1.0f, 1.5f, 1.5f, 0.8f, 0.2f } },
    { "Temperate Broadleaf", { 2.0f, 2.0f, 2.0f, 1.5f, 1.0f, 0.5f, 0.1f } },
    { "Subtropical Evergreen Forest", { 2.0f, 2.0f, 2.0f, 1.5f, 1.0f, 0.5f, 0.1f } },
    { "Mediterranean", { 1.5f, 2.0f, 1.5f, 1.2f, 1.0f, 0.6f, 0.1f } },
    { "Xeric Shrubland", { 1.0f, 0.7f, 0.5f, 0.5f, 0.2f, 0.3f, 0.0f } },
    { "Dry Forest and Woodland Savanna", { 2.0f, 2.5f, 1.5f, 1.8f, 1.2f, 0.5f, 0.1f } },
    { "Hot Arid Desert", { 3.0f, 0.5f, 0.5f, 0.2f, 0.1f, 0.1f, 0.0f } },
    { "Tundra", { 3.0f, 0.8f, 1.0f, 1.0f, 2.5f, 1.5f, 0.2f } },
    { "Montane Forests and Grasslands", { 2.5f, 2.0f, 1.5f, 1.0f, 3.0f, 1.0f, 0.1f } },
    { "Taiga (Boreal Forest)", { 3.0f, 1.5f, 2.0f, 1.5f, 2.0f, 0.8f, 0.1f } },
    { "Cold or Polar Desert", { 1.0f, 0.5f, 0.3f, 0.2f, 3.0f, 1.0f, 0.0f } }
};


FString CalculateBiomeProbabilities(
    float AdjustedTemperature, float Precipitation, float Humidity,
    float Latitude, float Altitude, float Slope, float Aspect,
    TArray<FString> Candidates)
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
        Scores[Index] = FMath::Max(0.0f, 
            Weights.TempWeight * AdjustedTemperature +
            Weights.PrecWeight * Precipitation +
            Weights.HumidityWeight * Humidity +
            Weights.LatitudeWeight * FMath::Abs(Latitude) +
            Weights.AltitudeWeight * Altitude +
            Weights.SlopeWeight * Slope +
            Weights.AspectWeight * Aspect);
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

/*

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
}*/