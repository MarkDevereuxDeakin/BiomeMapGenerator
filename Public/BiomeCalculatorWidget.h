#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Widget for displaying biome calculation results.
 */
class BIOMEMAPPER_API BiomeCalculatorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeCalculatorWidget) {}
    SLATE_END_ARGS()

    /**
     * Construct the widget.
     * @param InArgs - Slate arguments.
     */
    void Construct(const FArguments& InArgs);

    /**
     * Set the heightmap data to be displayed.
     * @param InHeightmapData - The heightmap data array.
     */
    void SetHeightmapData(const TArray<FHeightmapCell>& InHeightmapData);

private:
    /** Array of heightmap cells. */
    TArray<FHeightmapCell> HeightmapData;
};
