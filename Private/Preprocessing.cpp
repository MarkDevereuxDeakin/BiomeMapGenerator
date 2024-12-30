#include "Preprocessing.h"
#include "Async/ParallelFor.h"
#include "DistanceToOcean.h"
#include "PlanetTime.h"
#include "UnifiedWindCalculator.h"
#include "WindUtils.h"
#include "Humidity.h"
#include "Precipitation.h"
#include "Temperature.h"
#include "OceanCurrents.h"
#include "OceanTemperature.h"
#include "Albedo.h"
#include "SlopeAndAspect.h"
#include "Misc/FileHelper.h"

float ALBEDO_EFFECT = 5.0f;         // Albedo effect on temperature (°C)

bool Preprocessing::PreprocessData(TArray<FHeightmapCell>& HeightmapData, int32 Width, int32 Height)
{
    // Distance to Ocean and Closest Ocean Cell
    TArray<float> DistanceMap;
    TArray<int32> ClosestOceanIndices;

    // Initialize PlanetTime Singleton
    FPlanetTime& PlanetTime = FPlanetTime::GetInstance();
    const int32 YearLength = PlanetTime.GetYearLength();
    const int32 DayLength = PlanetTime.GetDayLengthSeconds();
    const int32 DayOfYear = PlanetTime.GetDayOfYear();

    // Compute Closest Ocean Cells
    if (!FindClosestOceanCell(HeightmapData, Width, Height, DistanceMap, ClosestOceanIndices))
    {
        return false;
    }

    // Precompute OceanToLandVectors
    ParallelFor(HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];
        if (ClosestOceanIndices[i] != -1)
        {
            const FHeightmapCell& NearestOceanCell = HeightmapData[ClosestOceanIndices[i]];
            Cell.OceanToLandVector = FVector2D(
                Cell.Longitude - NearestOceanCell.Longitude,
                Cell.Latitude - NearestOceanCell.Latitude
            ).GetSafeNormal();
        }
        else
        {
            Cell.OceanToLandVector = FVector2D::ZeroVector;
        }
    });

    //Calculate Slope and Aspect for each Heightmap Cell
    SlopeAndAspect::CalculateSlopeAndAspect(HeightmapData, Width, Height);

    // Main ParallelFor Loop
    ParallelFor(HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];

        // Calculate Wind Direction and Onshore Wind
        Cell.WindDirection = UnifiedWindCalculator::CalculateRefinedWind(Cell.Latitude, Cell.Longitude, 0.0f);
        Cell.IsWindOnshore = WindUtils::IsOnshoreWind(Cell.WindDirection, Cell.OceanToLandVector);

        // Calculate Relative Humidity
        Cell.RelativeHumidity = Humidity::CalculateRelativeHumidity(Cell.Latitude, Cell.DistanceToOcean, Cell.Altitude, Cell.IsWindOnshore);

        // Base Temperature Calculation
        Cell.Temperature = Temperature::CalculateSurfaceTemperature(
            Cell.Latitude, Cell.Altitude, DayOfYear, Cell.RelativeHumidity, PlanetTime, Cell.Slope, Cell.Aspect, Cell.WindDirection.Size());

        // Determine Flow Directions
        Cell.FlowDirection = OceanCurrents::ValidateFlowDirection(Cell.Latitude, Cell.Longitude, Cell.FlowDirection);

        // Adjust Temperature for Ocean Effects
        Cell.Temperature = OceanTemperature::CalculateOceanTemp(
            Cell.Temperature, Cell.DistanceToOcean, Cell.Latitude, Cell.Longitude, Cell.FlowDirection);

        // Calculate Precipitation
        Cell.AnnualPrecipitation = Precipitation::CalculatePrecipitation(
            Cell.Latitude, Cell.Altitude, Cell.DistanceToOcean, Cell.RelativeHumidity, Cell.Slope, Cell.WindDirection, Cell.OceanToLandVector);

        // Adjust Climate Factors
        WindUtils::AdjustWeatherFactors(
            Cell.IsWindOnshore, Cell.WindDirection.Size(), Cell.AnnualPrecipitation, 
            Cell.Temperature, Cell.DistanceToOcean);
        
    });

    // Calculate Albedo dynamically after adjusting weather factors
    Albedo::CalculateDynamicAlbedo(HeightmapData);

    // Adjust Temperature using Albedo
    ParallelFor(HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];
        Cell.Temperature -= Cell.Albedo * ALBEDO_EFFECT; // Subtract albedo effect
    });

    return true;
}
