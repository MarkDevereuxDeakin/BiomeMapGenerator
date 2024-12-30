#include "SlopeAndAspect.h"
#include "HeightmapCell.h"
#include "Math/UnrealMathUtility.h"
#include "Async/ParallelFor.h"

void SlopeAndAspect::CalculateSlopeAndAspect(
    TArray<FHeightmapCell>& HeightmapData,
    int32 Width,
    int32 Height)
{
    const TArray<FVector2D> NeighborOffsets = {
        FVector2D(-1, -1), FVector2D(0, -1), FVector2D(1, -1),
        FVector2D(-1,  0),                  FVector2D(1,  0),
        FVector2D(-1,  1), FVector2D(0,  1), FVector2D(1,  1)
    };

    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        int32 X = Index % Width;
        int32 Y = Index / Width;

        FHeightmapCell& Cell = HeightmapData[Index];
        float CurrentElevation = Cell.Altitude;

        float MaxSlope = 0.0f;
        float GradientX = 0.0f;
        float GradientY = 0.0f;

        for (const FVector2D& Offset : NeighborOffsets)
        {
            int32 NeighborX = X + Offset.X;
            int32 NeighborY = Y + Offset.Y;

            if (NeighborX >= 0 && NeighborX < Width && NeighborY >= 0 && NeighborY < Height)
            {
                int32 NeighborIndex = NeighborY * Width + NeighborX;
                const FHeightmapCell& NeighborCell = HeightmapData[NeighborIndex];
                float NeighborElevation = NeighborCell.Altitude;

                float Distance = Offset.Size();
                float Gradient = (NeighborElevation - CurrentElevation) / Distance;

                // Update X and Y gradients
                GradientX += Gradient * Offset.X;
                GradientY += Gradient * Offset.Y;

                // Calculate slope
                MaxSlope = FMath::Max(MaxSlope, FMath::Abs(Gradient));
            }
        }

        // Compute slope (in degrees)
        Cell.Slope = FMath::Atan(MaxSlope) * (180.0f / PI);

        // Compute aspect (in degrees)
        float Aspect = FMath::Atan2(GradientY, -GradientX) * (180.0f / PI);
        Cell.Aspect = FMath::Fmod(Aspect + 360.0f, 360.0f); // Normalize to [0, 360]
    });
}
