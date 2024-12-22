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
#include "Misc/FileHelper.h"

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

    // Main ParallelFor Loop
    ParallelFor(HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];

        // Calculate Wind Direction and Onshore Wind
        Cell.WindDirection = UnifiedWindCalculator::CalculateRefinedWind(Cell.Latitude, Cell.Longitude, 0.0f);
        Cell.IsWindOnshore = WindUtils::IsOnshoreWind(Cell.WindDirection, Cell.OceanToLandVector);

        // Calculate Relative Humidity
        Cell.RelativeHumidity = Humidity::CalculateRelativeHumidity(Cell.Latitude, Cell.DistanceToOcean, Cell.IsWindOnshore);

        // Base Temperature Calculation
        Cell.Temperature = Temperature::CalculateSurfaceTemperature(
            Cell.Latitude, Cell.Altitude, DayOfYear, Cell.RelativeHumidity, PlanetTime);

        // Determine Flow Directions
        Cell.FlowDirection = OceanCurrents::ValidateFlowDirection(Cell.Latitude, Cell.Longitude, Cell.FlowDirection);

        // Adjust Temperature for Ocean Effects
        Cell.Temperature = OceanTemperature::CalculateOceanTemp(
            Cell.Temperature, Cell.DistanceToOcean, Cell.Latitude, Cell.Longitude, Cell.FlowDirection);

        // Calculate Precipitation
        Cell.AnnualPrecipitation = Precipitation::CalculatePrecipitation(
            Cell.Latitude, Cell.Altitude, Cell.DistanceToOcean, Cell.RelativeHumidity);

        // Adjust Climate Factors
        WindUtils::AdjustWeatherFactors(
            Cell.IsWindOnshore, Cell.WindDirection.Size(), Cell.AnnualPrecipitation, 
            Cell.Temperature, Cell.DistanceToOcean);
    });

    return true;
}
