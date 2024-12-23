#include "DInfinity.h"
#include "HeightmapCell.h"
#include "Async/ParallelFor.h"
#include "Math/Vector2D.h"
#include "Math/UnrealMathUtility.h"

void DInfinity::CalculateFlowDirection(
    const TArray<FHeightmapCell>& Data,
    int32 Width,
    int32 Height,
    TArray<FVector2D>& OutFlowDirections)
{
    OutFlowDirections.Empty(Data.Num());
    OutFlowDirections.SetNum(Data.Num());

    const TArray<FVector2D> NeighborOffsets = {
        FVector2D(-1, -1), FVector2D(0, -1), FVector2D(1, -1),
        FVector2D(-1,  0),                  FVector2D(1,  0),
        FVector2D(-1,  1), FVector2D(0,  1), FVector2D(1,  1)
    };

    ParallelFor(Data.Num(), [&](int32 Index)
    {
        int32 x = Index % Width;
        int32 y = Index / Width;

        float CurrentElevation = Data[Index].Altitude;
        FVector2D BestDirection = FVector2D::ZeroVector;
        float MaxSlope = 0.0f;

        for (const FVector2D& Offset : NeighborOffsets)
        {
            int32 NeighborX = x + Offset.X;
            int32 NeighborY = y + Offset.Y;

            if (NeighborX >= 0 && NeighborX < Width && NeighborY >= 0 && NeighborY < Height)
            {
                int32 NeighborIndex = NeighborY * Width + NeighborX;
                float NeighborElevation = Data[NeighborIndex].Altitude;

                float Distance = Offset.Size();
                float Slope = (CurrentElevation - NeighborElevation) / Distance;

                if (Slope > MaxSlope)
                {
                    MaxSlope = Slope;
                    BestDirection = Offset / Distance;
                }
            }
        }

        OutFlowDirections[Index] = BestDirection;
    });
}

void DInfinity::CalculateFlowAccumulation(
    const TArray<FHeightmapCell>& Data,
    const TArray<FVector2D>& FlowDirections,
    int32 Width,
    int32 Height,
    TArray<float>& OutAccumulation)
{
    OutAccumulation.Empty(Data.Num());
    OutAccumulation.SetNumZeroed(Data.Num());

    TArray<int32> CellOrder;
    CellOrder.Reserve(Data.Num());

    for (int32 i = 0; i < Data.Num(); ++i)
    {
        CellOrder.Add(i);
    }

    CellOrder.Sort([&Data](int32 A, int32 B) {
        return Data[A].Altitude > Data[B].Altitude;
    });

    for (int32 Index : CellOrder)
    {
        FVector2D FlowDir = FlowDirections[Index];
        if (FlowDir.IsZero()) continue;

        int32 FlowX = FMath::RoundToInt(Index % Width + FlowDir.X);
        int32 FlowY = FMath::RoundToInt(Index / Width + FlowDir.Y);

        if (FlowX >= 0 && FlowX < Width && FlowY >= 0 && FlowY < Height)
        {
            int32 NeighborIndex = FlowY * Width + FlowX;
            OutAccumulation[NeighborIndex] += OutAccumulation[Index] + 1.0f;
        }
    }
}

void DInfinity::CalculateFlowDepth(
    const TArray<FHeightmapCell>& Data,
    const TArray<float>& FlowAccumulation,
    const TArray<float>& EffectiveRiverWidths,
    int32 Width,
    int32 Height,
    TArray<float>& OutFlowDepth,
    const TArray<float>& PrecipitationFactors)
{
    OutFlowDepth.Empty(Data.Num());
    OutFlowDepth.SetNumZeroed(Data.Num());

    ParallelFor(Data.Num(), [&](int32 Index)
    {
        float Accumulation = FlowAccumulation[Index];
        float EffectiveRiverWidth = EffectiveRiverWidths[Index];
        float PrecipitationFactor = PrecipitationFactors[Index];

        if (EffectiveRiverWidth > 0)
        {
            OutFlowDepth[Index] = Accumulation * PrecipitationFactor / EffectiveRiverWidth;
        }
    });
}
