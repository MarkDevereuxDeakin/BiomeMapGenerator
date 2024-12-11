#pragma once
#include "CoreMinimal.h"
#include "DInfinity.generated.h"

UCLASS()
class BIOMEMAPPER_API UDInfinity : public UObject
{
    GENERATED_BODY()
public:
    // Computes D-Infinity flow direction
    static void CalculateFlowDirection(
        const TArray<FHeightmapCell>& Data, 
        int32 Width, 
        int32 Height, 
        TArray<FVector2D>& OutFlowDirections);

    // Calculates flow accumulation
    static void CalculateFlowAccumulation(
        const TArray<FHeightmapCell>& Data, 
        const TArray<FVector2D>& FlowDirections, 
        int32 Width, 
        int32 Height, 
        TArray<float>& OutAccumulation);
};
