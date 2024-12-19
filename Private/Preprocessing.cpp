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

    // Date Calculation based on user input
    FPlanetTime& PlanetTime = FPlanetTime::GetInstance();

    // Get day of year from planetary time    
    int32 YearLength = PlanetTime.GetYearLength();
    int32 DayLength = PlanetTime.GetDayLengthSeconds();
    int32 DayOfYear = PlanetTime.GetDayOfYear();

    if (!FindClosestOceanCell(HeightmapData, Width, Height, DistanceMap, ClosestOceanIndices))
    {
        return false;
    }

    ParallelFor (HeightmapData.Num(), [&](int32 i)
    {
        FHeightmapCell& Cell = HeightmapData[i];        

        // Calculate wind direction based on latitude, longitude, day of the year and time of day.        
        Cell.WindDirection = UnifiedWindCalculator::CalculateRefinedWind(Cell.Latitude, Cell.Longitude, 0.0f);

        // Determine if the wind is onshore
        Cell.IsWindOnshore = WindUtils::IsOnshoreWind(Cell.WindDirection, Cell.OceanToLandVector);// Should this be Cell.WindDirection

        // Calculate relative humidity
        Cell.RelativeHumidity = Humidity::CalculateRelativeHumidity(Cell.Latitude, Cell.DistanceToOcean, Cell.IsWindOnshore);

        //Calculate base temperature for a cell.
        Cell.Temperature = Temperature::CalculateSurfaceTemperature(Cell.Latitude, Cell.Altitude, DayOfYear, Cell.RelativeHumidity, PlanetTime);

        // Determine Flow Directions
        Cell.FlowDirection = OceanCurrents::ValidateFlowDirection(Cell.Latitude, Cell.Longitude, Cell.FlowDirection);

        // Adjust temperature based on ocean effects
        Cell.Temperature = OceanTemperature::CalculateOceanTemp(Cell.Temperature, Cell.DistanceToOcean, Cell.Latitude, Cell.Longitude, Cell.FlowDirection);
        
        // Calculate precipitation
        Cell.AnnualPrecipitation = ::Precipitation::CalculatePrecipitation(Cell.Latitude, Cell.Altitude, Cell.DistanceToOcean, Cell.RelativeHumidity);

        // Adjust Climate Factors
        WindUtils::AdjustWeatherFactors(Cell.IsWindOnshore, Cell.WindDirection.Size(), Cell.AnnualPrecipitation, Cell.Temperature, Cell.DistanceToOcean);
    });

    return true;
}
