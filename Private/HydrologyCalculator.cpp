#include "HydrologyCalculator.h"
#include "DInfinity.h"
#include "Precipitation.h"
#include "Temperature.h"
#include "Humidity.h"
#include "UnifiedWindCalculator.h"
#include "PlanetTime.h"
#include "HeightmapCell.h"

// Helper function to validate and adjust flow accumulation based on environment
void HydrologyCalculator::AdjustFlowForEnvironment(
    const FHeightmapCell& Cell,
    float& FlowAccumulation,
    int DayOfYear,
    const FPlanetTime& PlanetTime){

    // Snowmelt and Freeze Logic
    // Dynamic altitude threshold for freezing conditions
const float FreezingAltitude = 2000.0f; // This could be adjusted dynamically based on regional data

    // Snowmelt and Freeze Logic
    if (Cell.Temperature < 0.0f) // Below freezing
    {
        // Gradual freezing effect, reducing flow more as temperature decreases
        float FreezeFactor = FMath::Clamp(1.0f + (Cell.Temperature / 10.0f), 0.0f, 1.0f); // Scaled by temperature
        FlowAccumulation *= FreezeFactor; // Reduce flow gradually
    }
    else if (Cell.Temperature > 0.0f && Cell.Altitude > FreezingAltitude) // Snowmelt at high altitude
    {
        // Gradual snowmelt based on temperature above freezing
        float MeltFactor = FMath::Clamp(Cell.Temperature / 10.0f, 0.0f, 1.0f); // Scaled by temperature
        FlowAccumulation += Cell.AnnualPrecipitation * MeltFactor; // Increase flow with meltwater
    }

    // Evaporation Effect
    float EvaporationFactor = 1.0f - (0.02f * Humidity::CalculateRelativeHumidity(
        Cell.Latitude, Cell.DistanceToOcean, true
    ));

    FlowAccumulation *= FMath::Clamp(EvaporationFactor, 0.0f, 1.0f);
}

float HydrologyCalculator::CalculateEffectiveRiverWidth(const FHeightmapCell& Cell, float Slope)
{
    // Base width for rivers (land cells with flow convergence)
    float BaseWidth = 10.0f; // Default width in meters

    // Adjust based on distance to ocean (closer -> wider rivers)
    float OceanEffect = FMath::Clamp(1.0f - (Cell.DistanceToOcean / 500.0f), 0.5f, 2.0f);

    // Adjust based on slope (steeper slopes -> narrower rivers)
    float SlopeEffect = FMath::Clamp(1.0f / (Slope + 0.1f), 0.5f, 2.0f);

    // Wider rivers for pre-identified river cells
    if (Cell.CellType == ECellType::River)
    {
        BaseWidth *= 1.5f;
    }

    return BaseWidth * OceanEffect * SlopeEffect;
}


float HydrologyCalculator::CalculateStreamThreshold(const FHeightmapCell& Cell, FVector2D Resolution)
{
    // Base threshold determined by resolution
    float BaseThreshold = 100.0f / (Resolution.X + Resolution.Y); // Adjust for pixels per degree

    // Adjust based on precipitation
    float PrecipitationEffect = FMath::Clamp(Cell.AnnualPrecipitation / 1000.0f, 0.1f, 2.0f);

    // Adjust based on proximity to ocean
    float OceanEffect = FMath::Clamp(1.0f - (Cell.DistanceToOcean / 1000.0f), 0.1f, 1.0f);

    // Adjust based on humidity (higher humidity -> lower threshold for stream formation)
    float HumidityEffect = FMath::Clamp(1.0f - Cell.RelativeHumidity, 0.1f, 1.0f);

    // Combine factors
    return BaseThreshold * PrecipitationEffect * OceanEffect * HumidityEffect;
}

bool HydrologyCalculator::AnalyzeHydrology(
    const TArray<FHeightmapCell>& HeightmapData,
    int32 Width,
    int32 Height,
    FVector2D Resolution,
    TArray<FVector2D>& OutFlowDirections,
    TArray<float>& OutFlowAccumulation,
    TArray<float>& OutFlowDepth,
    float& MaxFlowDepth, 
    float StreamThreshold,
    TMap<int32, TArray<FVector>>& OutSeasonalStreamPaths)
{
    if (HeightmapData.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid heightmap data dimensions."));
        return false;
    }

    // Date Calculation based on user input
    FPlanetTime& PlanetTime = FPlanetTime::GetInstance();

    // Get day of year from planetary time    
    int32 YearLength = PlanetTime.GetYearLength();
    int32 DayOfYear = PlanetTime.GetDayOfYear();
    float TimeOfYear = DayOfYear/YearLength;

    // Step 1: Calculate flow direction
    DInfinity::CalculateFlowDirection(HeightmapData, Width, Height, OutFlowDirections);

    // Step 2: Calculate flow accumulation
    DInfinity::CalculateFlowAccumulation(HeightmapData, OutFlowDirections, Width, Height, OutFlowAccumulation);

    // Step 3: Calculate Effective River Widths
    TArray<float> EffectiveRiverWidths;
    EffectiveRiverWidths.SetNum(HeightmapData.Num());

    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        const FHeightmapCell& Cell = HeightmapData[Index];
        FVector2D FlowDirection = OutFlowDirections[Index];
        float Slope = FMath::Abs(FlowDirection.X + FlowDirection.Y);
        EffectiveRiverWidths[Index] = HydrologyCalculator::CalculateEffectiveRiverWidth(Cell, Slope);
    });

     // Step 4: Precompute PrecipitationFactor
    TArray<float> PrecipitationFactors;
    PrecipitationFactors.SetNum(HeightmapData.Num());

    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        const FHeightmapCell& Cell = HeightmapData[Index];
        PrecipitationFactors[Index] = FMath::Clamp(Cell.AnnualPrecipitation / 1000.0f, 0.1f, 2.0f);
    });

    // Step 5: Calculate Flow Depth using effective river widths
    DInfinity::CalculateFlowDepth(
        HeightmapData,
        OutFlowAccumulation,
        EffectiveRiverWidths, // Use precomputed values
        Width,
        Height,
        OutFlowDepth,
        PrecipitationFactors);

    // Find MaxFlowDepth
    // ParallelFor and/or Sort function?
    MaxFlowDepth = 0.0f;
    for (float Depth : OutFlowDepth)
    {
        if (Depth > MaxFlowDepth)
        {
            MaxFlowDepth = Depth;
        }
    }

    // Step 5: Environmental adjustments
    TArray<float> AdjustedFlowAccumulation = OutFlowAccumulation;
    

   // Parallel environmental adjustment
    ParallelFor(HeightmapData.Num(), [&](int32 Index)
    {
        AdjustFlowForEnvironment(
            HeightmapData[Index],
            AdjustedFlowAccumulation[Index],
            DayOfYear,
            PlanetTime);
    });

    // Step 6: Seasonal adjustments
    FCriticalSection MapMutex;
    ParallelFor(4, [&](int32 Season)
    {
        TArray<FVector> StreamPaths;
        float SeasonalPrecipitationMultiplier = 1.0f + 0.2f * FMath::Sin((Season + TimeOfYear) * PI / 2.0f);

        // Temporary storage for seasonal flow depth
        TArray<float> SeasonalFlowDepth;
        SeasonalFlowDepth.SetNumZeroed(Width * Height);

        for (int32 y = 0; y < Height; ++y)
        {
            for (int32 x = 0; x < Width; ++x)
            {
                int32 Index = y * Width + x;
                const FHeightmapCell& Cell = HeightmapData[Index];                

                // Validate precipitation value for the current cell
                float PrecipitationFactor = PrecipitationFactors[Index];

                // Adjust flow accumulation for the season
                float AdjustedAccumulation = AdjustedFlowAccumulation[Index] * SeasonalPrecipitationMultiplier;

                // Use precomputed EffectiveRiverWidths for seasonal depth calculation
                float EffectiveRiverWidth = EffectiveRiverWidths[Index];
                if (EffectiveRiverWidth > 0.0f)
                {
                    SeasonalFlowDepth[Index] = AdjustedAccumulation * PrecipitationFactor / EffectiveRiverWidth;
                }

                // Calculate stream threshold dynamically
                float CellStreamThreshold = HydrologyCalculator::CalculateStreamThreshold(Cell, Resolution);

                // Check if the flow depth meets the stream threshold
                if (SeasonalFlowDepth[Index] >= CellStreamThreshold)
                {
                    FVector StreamPoint = FVector(x, y, Cell.Altitude);
                    StreamPaths.Add(StreamPoint);
                }
            }
        }

        // Lock the critical section and update seasonal stream paths
        FScopeLock Lock(&MapMutex);
        OutSeasonalStreamPaths.Add(Season, StreamPaths);
    });

    return true;
}

TArray<FColor> HydrologyCalculator::GenerateVisualizationTexture(
    const TMap<int32, TArray<FVector>>& SeasonalStreamPaths,
    const TArray<FHeightmapCell>& HeightmapData,
    const TArray<float>& FlowDepth,
    float MaxFlowDepth,
    int32 Width,
    int32 Height,
    float MinAltitudeSlider,
    float MaxAltitudeSlider,
    TArray<FColor>& OutTextureColors,
    TArray<FVector>& OutTextureVectors)
{
    // Initialize outputs
    OutTextureColors.Init(FColor::Black, Width * Height); // Initialize with black as a fallback
    OutTextureVectors.Init(FVector::ZeroVector, Width * Height);    

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;

            // Get the corresponding heightmap cell
            const FHeightmapCell& Cell = HeightmapData[Index];

            // Calculate grayscale/colour data value based on altitude. (Scalar-based visualization)
            uint8 GrayValue = static_cast<uint8>(FMath::Clamp(
                (Cell.Altitude - MinAltitudeSlider) / (MaxAltitudeSlider - MinAltitudeSlider) * 255.0f, 0.0f, 255.0f));

            // Start with the base grayscale color
            OutTextureColors[Index] = FColor(GrayValue, GrayValue, GrayValue, 255);

            // Overlay streams with blue based on flow depth
            if (FlowDepth[Index] > 0.0f)
            {
                uint8 BlueIntensity = static_cast<uint8>(FMath::Clamp(
                    FlowDepth[Index] / MaxFlowDepth * 255.0f, 0.0f, 255.0f));
                OutTextureColors[Index] = FColor(0, 0, BlueIntensity, 255);
            }

            // Generate vector data (for future functionality)
            FVector VectorData(Cell.Longitude, Cell.Latitude, Cell.Altitude);
            OutTextureVectors[Index] = VectorData;
        }
    }

    // Add seasonal stream paths
    for (const auto& Pair : SeasonalStreamPaths)
    {
        for (const FVector& StreamPoint : Pair.Value)
        {
            int32 X = FMath::Clamp(static_cast<int32>(StreamPoint.X), 0, Width - 1);
            int32 Y = FMath::Clamp(static_cast<int32>(StreamPoint.Y), 0, Height - 1);
            int32 Index = Y * Width + X;

           // Overlay blue streams on scalar texture
            OutTextureColors[Index] = FColor(0, 0, 255, 255);

            // Add stream points to vector data
            OutTextureVectors[Index] = StreamPoint;
        }
    }

    // Return the color data
    return OutTextureColors;
    
}