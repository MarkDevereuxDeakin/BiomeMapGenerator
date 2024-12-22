#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class BIOMEMAPPER_API HydrologyCalculator
{

public:
    
    static bool AnalyzeHydrology(
    const TArray<FHeightmapCell>& HeightmapData,
    int32 Width,
    int32 Height,
    FVector2D Resolution,
    TArray<FVector2D>& OutFlowDirections,
    TArray<float>& OutFlowAccumulation,
    TArray<float>& OutFlowDepth,
    float& MaxFlowDepth,
    float StreamThreshold,
    TMap<int32, TArray<FVector>>& OutSeasonalStreamPaths);

    
    static void AdjustFlowForEnvironment(
    const FHeightmapCell& Cell,
    float& FlowAccumulation,
    const float HumidityFactor,
    int DayOfYear,
    const FPlanetTime& PlanetTime);

    
    static TArray<FColor> GenerateVisualizationTexture(
    const TMap<int32, TArray<FVector>>& StreamPaths,
    const TArray<FHeightmapCell>& HeightmapData,    
    const TArray<float>& FlowDepth,
    float MaxFlowDepth,
    int32 Width,
    int32 Height,
    float MinxAltitudeSlider,
    float MaxAltitudeSlider,    
    TArray<FColor>& TextureColors,
    TArray<FVector>& TextureVectors);

    
    static float CalculateStreamThreshold(const FHeightmapCell& Cell, FVector2D Resolution);

    
    static float CalculateEffectiveRiverWidth(const FHeightmapCell& Cell, float Slope);

};
