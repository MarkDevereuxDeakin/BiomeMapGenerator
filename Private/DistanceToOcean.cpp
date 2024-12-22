#include "DistanceToOcean.h"
#include "Math/UnrealMathUtility.h"
#include "Async/ParallelFor.h"
/*
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
}*/


bool FindClosestOceanCell(
    const TArray<FHeightmapCell>& Data,
    int32 Width,
    int32 Height,
    TArray<float>& OutDistanceMap,
    TArray<int32>& OutClosestOceanIndex)
{
    if (Data.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid data dimensions for closest ocean cell calculation."));
        return false;
    }

    // Initialize distance map and closest ocean index map
    OutDistanceMap.Init(FLT_MAX, Data.Num());
    OutClosestOceanIndex.Init(-1, Data.Num());

    TQueue<int32> Queue;

    // Enqueue all ocean cells
    ParallelFor(Data.Num(), [&](int32 Index)
    {
        if (Data[Index].OceanDepth > 0.0f) // Ocean cell
        {
            OutDistanceMap[Index] = 0.0f;
            OutClosestOceanIndex[Index] = Index; // The cell itself is the closest ocean cell
            Queue.Enqueue(Index);
        }
    });

    // Neighbor offsets
    const TArray<FIntPoint> Offsets = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0) };

    // BFS for distance calculation
    while (!Queue.IsEmpty())
    {
        int32 CurrentIndex;
        Queue.Dequeue(CurrentIndex);

        int32 CurrentX = CurrentIndex % Width;
        int32 CurrentY = CurrentIndex / Width;
        float CurrentDistance = OutDistanceMap[CurrentIndex];
        int32 ClosestOceanIndex = OutClosestOceanIndex[CurrentIndex];

        for (const FIntPoint& Offset : Offsets)
        {
            int32 NeighborX = CurrentX + Offset.X;
            int32 NeighborY = CurrentY + Offset.Y;

            if (NeighborX >= 0 && NeighborX < Width && NeighborY >= 0 && NeighborY < Height)
            {
                int32 NeighborIndex = NeighborY * Width + NeighborX;

                if (OutDistanceMap[NeighborIndex] > CurrentDistance + 1.0f) // Update distance
                {
                    OutDistanceMap[NeighborIndex] = CurrentDistance + 1.0f;
                    OutClosestOceanIndex[NeighborIndex] = ClosestOceanIndex;
                    Queue.Enqueue(NeighborIndex);
                }
            }
        }
    }

    return true;
}

bool CalculateDistanceToOcean(TArray<FHeightmapCell>& Data, int32 Width, int32 Height)
{
    if (Data.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid data dimensions for distance calculation."));
        return false;
    }

    TArray<float> DistanceMap;
    TArray<int32> ClosestOceanIndex;

    // Find the closest ocean cells
    if (!FindClosestOceanCell(Data, Width, Height, DistanceMap, ClosestOceanIndex))
    {
        return false;
    }

    // Assign distances and calculate OceanToLandVector
    ParallelFor(Data.Num(), [&](int32 Index)
    {
        // Assign DistanceToOcean
        Data[Index].DistanceToOcean = DistanceMap[Index];

        // Cache longitude and latitude for efficiency
        float CellLongitude = Data[Index].Longitude;
        float CellLatitude = Data[Index].Latitude;

        // Calculate OceanToLandVector
        if (ClosestOceanIndex[Index] != -1) // Ensure there's a valid nearest ocean cell
        {
            const FHeightmapCell& NearestOceanCell = Data[ClosestOceanIndex[Index]];
            Data[Index].OceanToLandVector = FVector2D(
                CellLongitude - NearestOceanCell.Longitude,
                CellLatitude - NearestOceanCell.Latitude
            ).GetSafeNormal();
        }
        else
        {
            Data[Index].OceanToLandVector = FVector2D::ZeroVector;
        }
    });

    return true;
}