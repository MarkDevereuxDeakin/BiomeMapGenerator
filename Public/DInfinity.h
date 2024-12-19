#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Math/Vector2D.h"

/**
 * Utility class for hydrological calculations using the D-Infinity method.
 */
class BIOMEMAPPER_API DInfinity
{
public:
    /**
     * Computes D-Infinity flow direction for each heightmap cell.
     * @param Data - Input heightmap data.
     * @param Width - Width of the heightmap.
     * @param Height - Height of the heightmap.
     * @param OutFlowDirections - Output array for storing flow directions.
     */
    static void CalculateFlowDirection(
        const TArray<FHeightmapCell>& Data,
        int32 Width,
        int32 Height,
        TArray<FVector2D>& OutFlowDirections);

    /**
     * Calculates flow accumulation for each heightmap cell.
     * @param Data - Input heightmap data.
     * @param FlowDirections - Precomputed flow directions.
     * @param Width - Width of the heightmap.
     * @param Height - Height of the heightmap.
     * @param OutAccumulation - Output array for storing flow accumulation.
     */
    static void CalculateFlowAccumulation(
        const TArray<FHeightmapCell>& Data,
        const TArray<FVector2D>& FlowDirections,
        int32 Width,
        int32 Height,
        TArray<float>& OutAccumulation);

    /**
     * Calculates flow depth for each heightmap cell.
     * @param Data - Input heightmap data.
     * @param FlowAccumulation - Precomputed flow accumulation.
     * @param EffectiveRiverWidths - Effective river widths for each cell.
     * @param Width - Width of the heightmap.
     * @param Height - Height of the heightmap.
     * @param OutFlowDepth - Output array for storing flow depth.
     * @param PrecipitationFactors - Factors for scaling precipitation influence.
     */
    static void CalculateFlowDepth(
        const TArray<FHeightmapCell>& Data,
        const TArray<float>& FlowAccumulation,
        const TArray<float>& EffectiveRiverWidths,
        int32 Width,
        int32 Height,
        TArray<float>& OutFlowDepth,
        const TArray<float>& PrecipitationFactors);
};
