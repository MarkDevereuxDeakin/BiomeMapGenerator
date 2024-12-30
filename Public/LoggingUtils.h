#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"

/**
 * Logs heightmap cell data and calculated biomes to a CSV file.
 * @param HeightmapData Array of FHeightmapCell objects.
 * @param FilePath File path to save the CSV.
 */
void LogBiomeDataToCSV(const TArray<FHeightmapCell>& HeightmapData, const FString& FilePath);
