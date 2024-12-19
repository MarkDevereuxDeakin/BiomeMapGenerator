#pragma once

#include "HeightmapCell.h"

bool FindClosestOceanCell(
    const TArray<FHeightmapCell>& Data,
    int32 Width,
    int32 Height,
    TArray<float>& OutDistanceMap,
    TArray<int32>& OutClosestOceanIndex);

/**
 * Calculate the distance of each cell to the nearest ocean.
 * @param Data - Array of heightmap cells.
 * @param Width - Width of the heightmap.
 * @param Height - Height of the heightmap.
 * @return True if calculation succeeded, false otherwise.
 */
bool CalculateDistanceToOcean(TArray<FHeightmapCell>& Data, int32 Width, int32 Height);