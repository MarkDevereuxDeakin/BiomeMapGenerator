#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Widgets/SCompoundWidget.h"

class BIOMEMAPPER_API BiomeCalculatorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeCalculatorWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetHeightmapData(const TArray<FHeightmapCell>& InHeightmapData);

private:
    TArray<FHeightmapCell> HeightmapData;
};