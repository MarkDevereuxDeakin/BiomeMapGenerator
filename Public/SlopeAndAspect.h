#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Math/Vector2D.h"

/**
 * Utility class for terrain analysis using the D-Infinity method.
 */
class BIOMEMAPPER_API SlopeAndAspect
{
public:
    /**
     * Computes slope and aspect for each heightmap cell.
     * @param Data - Input heightmap data.
     * @param Width - Width of the heightmap.
     * @param Height - Height of the heightmap.
     * @param OutSlopes - Output array for storing slope values (in degrees).
     * @param OutAspects - Output array for storing aspect values (in degrees).
     */
    static void CalculateSlopeAndAspect(
        TArray<FHeightmapCell>& Data,
        int32 Width,
        int32 Height);
};
