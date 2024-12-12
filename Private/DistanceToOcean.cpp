#include "DistanceToOcean.h"
#include "Math/UnrealMathUtility.h"

bool CalculateDistanceToOcean(TArray<FHeightmapCell>& Data, int32 Width, int32 Height)
{
    if (Data.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid data dimensions for distance calculation."));
        return false;
    }

    TArray<float> DistanceMap;
    DistanceMap.Init(FLT_MAX, Data.Num());

    TQueue<int32> Queue;

    // Enqueue all ocean cells
    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        if (Data[Index].OceanDepth > 0.0f) // Ocean cell
        {
            DistanceMap[Index] = 0.0f;
            Queue.Enqueue(Index);
        }
    }

    // Neighbor offsets
    const TArray<FIntPoint> Offsets = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0) };

    // BFS for distance calculation
    while (!Queue.IsEmpty())
    {
        int32 CurrentIndex;
        Queue.Dequeue(CurrentIndex);

        int32 CurrentX = CurrentIndex % Width;
        int32 CurrentY = CurrentIndex / Width;
        float CurrentDistance = DistanceMap[CurrentIndex];

        for (const FIntPoint& Offset : Offsets)
        {
            int32 NeighborX = CurrentX + Offset.X;
            int32 NeighborY = CurrentY + Offset.Y;

            if (NeighborX >= 0 && NeighborX < Width && NeighborY >= 0 && NeighborY < Height)
            {
                int32 NeighborIndex = NeighborY * Width + NeighborX;

                if (DistanceMap[NeighborIndex] > CurrentDistance + 1.0f) // Update distance
                {
                    DistanceMap[NeighborIndex] = CurrentDistance + 1.0f;
                    Queue.Enqueue(NeighborIndex);
                }
            }
        }
    }

    // Assign distances back to HeightmapCell
    ParallelFor(Data.Num(), [&](int32 Index)
    {
        Data[Index].DistanceToOcean = DistanceMap[Index];
    });

    return true;
}
