#include "ResultsWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"


void SResultsWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)
        // Result Display
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        [
            SAssignNew(ResultsTextBlock, STextBlock)
            .Text(FText::FromString("Results will be displayed here."))
        ]

        // Heightmap Image
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(HeightmapImage, SImage)
        ]        
    ];
}

void SResultsWidget::UpdateResults(const FString& ResultsText)
{
    if (ResultsTextBlock.IsValid())
    {
        ResultsTextBlock->SetText(FText::FromString(ResultsText));
    }
}

void SResultsWidget::UpdateHeightmapTexture(UTexture2D* HeightmapTexture)
{
    if (!HeightmapTexture)
    {
        return;
    }

    if (!HeightmapBrush.IsValid())
    {
        HeightmapBrush = MakeShareable(new FSlateBrush());
    }

    HeightmapBrush->SetResourceObject(HeightmapTexture);
    HeightmapBrush->ImageSize = FVector2D(HeightmapTexture->GetSizeX(), HeightmapTexture->GetSizeY());

    if (HeightmapImage.IsValid())
    {
        HeightmapImage->SetImage(HeightmapBrush.Get());
    }
}