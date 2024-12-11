#include "HydrologyCalculator.h"
#include "DInfinity.h"
#include "Precipitation.h"
#include "Temperature.h"
#include "Humidity.h"
#include "UnifiedWindCalculator.h"
#include "PlanetTime.h"

bool HydrologyCalculator::AnalyzeHydrology(
    const TArray<FHeightmapCell>& HeightmapData,
    int32 Width,
    int32 Height,
    TArray<FVector2D>& OutFlowDirections,
    TArray<float>& OutFlowAccumulation,
    float StreamThreshold,
    TMap<int32, TArray<FVector>>& OutSeasonalStreamPaths,
    float TimeOfYear)
{
    // Step 1: Calculate flow direction
    UDInfinity::CalculateFlowDirection(HeightmapData, Width, Height, OutFlowDirections);

    // Step 2: Calculate flow accumulation
    UDInfinity::CalculateFlowAccumulation(HeightmapData, OutFlowDirections, Width, Height, OutFlowAccumulation);

     // Step 3: Environmental adjustments
    TArray<float> AdjustedFlowAccumulation = OutFlowAccumulation;

    FPlanetTime PlanetTime(24.0f, 365.25f, 12);
    int DayOfYear = PlanetTime.GetDayOfYear();

    FCriticalSection MapMutex;

    ParallelFor (HeightmapData.Num(), [&](int32 Index)
    {
        const FHeightmapCell& Cell = HeightmapData[Index];
        float Precipitation = Precipitation::CalculatePrecipitation(
            Cell.Latitude,
            Cell.Altitude,
            Cell.DistanceToOcean, 
            Humidity::GetBaseHumidity(Cell.Latitude, Cell.DistanceToOcean)
        );

        float Temperature = Temperature::CalculateSurfaceTemperature(
            Cell.Latitude,
            Cell.Altitude,
            DayOfYear, 
            Humidity::GetBaseHumidity(Cell.Latitude, Cell.DistanceToOcean), 
            PlanetTime
        );

        // Snowmelt and Freeze Logic
        if (Temperature < 0.0f) // Water freezes
        {
            AdjustedFlowAccumulation[Index] *= 0.5f; // Reduce flow due to freezing
        }
        else if (Temperature > 0.0f && Cell.Altitude > 2000.0f) // Snowmelt above 2km
        {
            AdjustedFlowAccumulation[Index] += Precipitation * 0.3f; // Add meltwater
        }

        // Evaporation Effect
        float EvaporationFactor = 1.0f - (0.02f * Humidity::CalculateRelativeHumidity(
            Cell.Latitude, Cell.DistanceToOcean, true
        ));
        AdjustedFlowAccumulation[Index] *= FMath::Clamp(EvaporationFactor, 0.0f, 1.0f);
    });

    // Step 4: Seasonal adjustments
    ParallelFor (4, [&](int32 Season)
    {
        TArray<FVector> StreamPaths;
        float SeasonalPrecipitationMultiplier = 1.0f + 0.2f * FMath::Sin((Season + TimeOfYear) * PI / 2.0f);

        for (int32 y = 0; y < Height; ++y)
        {
            for (int32 x = 0; x < Width; ++x)
            {
                int32 Index = y * Width + x;
                if (AdjustedFlowAccumulation[Index] * SeasonalPrecipitationMultiplier >= StreamThreshold)
                {
                    FVector StreamPoint = FVector(x, y, HeightmapData[Index].Altitude);
                    StreamPaths.Add(StreamPoint);
                }
            }
        }
        // Safely add to the map in a thread-safe manner
        {
            FScopeLock Lock(&MapMutex);
            OutSeasonalStreamPaths.Add(Season, StreamPaths);
        }
    });

    return true;
}
