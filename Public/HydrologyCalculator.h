#pragma once

#include "CoreMinimal.h"

class BIOMEMAPPER_API HydrologyCalculator : public UObject
{

public:
    static bool AnalyzeHydrology(
        const TArray<FHeightmapCell>& HeightmapData,
        int32 Width,
        int32 Height,
        TArray<FVector2D>& OutFlowDirections,
        TArray<float>& OutFlowAccumulation,
        float StreamThreshold,
        TMap<int32, TArray<FVector>>& OutSeasonalStreamPaths,
        float TimeOfYear
    );

};
