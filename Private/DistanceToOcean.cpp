#include "DistanceToOcean.h"
#include "Math/UnrealMathUtility.h"

bool CalculateDistanceToOcean(TArray<FHeightmapCell>& Data, int32 Width, int32 Height)
{
    // Ensure data validity
    if (Data.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid data dimensions for distance calculation."));
        return false;
    }

    // Example logic for distance-to-ocean computation
    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;

            if (Data[Index].OceanDepth == 0.0f) // Ocean pixel
            {
                Data[Index].DistanceToOcean = 0.0f;
            }
            else
            {
                // Compute distance to the nearest ocean pixel (placeholder logic)
                Data[Index].DistanceToOcean = FLT_MAX; // Replace with real computation
            }
        }
    }

    return true;
}