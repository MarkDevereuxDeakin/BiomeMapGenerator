#include "DistanceToOcean.h"
#include "Math/UnrealMathUtility.h"

bool CalculateDistanceToOcean(TArray<FHeightmapCell>& Data, int32 Width, int32 Height)
{
     if (Data.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid data dimensions for distance calculation."));
        return false;
    }

    // Initialize distances and a queue for BFS
    TArray<float> DistanceMap;
    DistanceMap.Init(FLT_MAX, Data.Num());

    TQueue<int32> Queue;

    // Enqueue all ocean pixels
    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        if (Data[Index].OceanDepth > 0.0f) // Ocean pixel
        {
            DistanceMap[Index] = 0.0f;
            Queue.Enqueue(Index);
        }
    }

    // Neighbor offsets (4-directional)
    const TArray<FIntPoint> Offsets = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0) };

    // Perform BFS
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

    // Update the FHeightmapCell data
    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        Data[Index].DistanceToOcean = DistanceMap[Index];
    }

    return true;
}