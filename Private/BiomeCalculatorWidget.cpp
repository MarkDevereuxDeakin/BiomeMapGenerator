#include "BiomeCalculatorWidget.h"
#include "Widgets/Text/STextBlock.h"

void BiomeCalculatorWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(STextBlock).Text(FText::FromString("Biome Calculator Widget"))
    ];
}

void BiomeCalculatorWidget::SetHeightmapData(const TArray<FHeightmapCell>& InHeightmapData)
{
    HeightmapData = InHeightmapData;
}