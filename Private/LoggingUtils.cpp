#include "LoggingUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void LogBiomeDataToCSV(const TArray<FHeightmapCell>& HeightmapData, const FString& FilePath)
{
    FString FileContent = "CellIndex,Latitude,Longitude,Altitude,Temperature,Precipitation,Slope,Aspect,Biome\n";

    for (int32 CellIndex = 0; CellIndex < HeightmapData.Num(); ++CellIndex)
    {
        const FHeightmapCell& Cell = HeightmapData[CellIndex];

        if(!Cell.Altitude == 0)
        {
            FileContent += FString::Printf(
            TEXT("%d,%f,%f,%f,%f,%f,%f,%f,%s\n"),
            CellIndex,
            Cell.Latitude,
            Cell.Longitude,
            Cell.Altitude,
            Cell.Temperature,
            Cell.AnnualPrecipitation,
            Cell.Slope,
            Cell.Aspect,
            *Cell.BiomeType);
        }
    }

    if (FFileHelper::SaveStringToFile(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Biome data logged to file: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to write biome data to file: %s"), *FilePath);
    }
}
