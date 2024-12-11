// ButtonRowWidget.cpp
#include "ButtonRowWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"


void SButtonRowWidget::Construct(const FArguments& InArgs)
{
    OnUploadHeightmap = InArgs._OnUploadHeightmap; // Bind delegates
    OnCalculateBiome = InArgs._OnCalculateBiome; // Bind delegates
    OnAnalyzeHydrology = InArgs._OnAnalyzeHydrology; // Bind delegates

    ChildSlot
    [
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString("Upload Heightmap"))
            .OnClicked(this, &SButtonRowWidget::HandleUploadClicked)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString("Analyze Hydrology")) // New button
            .OnClicked(this, &SButtonRowWidget::HandleAnalyzeHydrologyClicked)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString("Calculate Biome"))
            .OnClicked(this, &SButtonRowWidget::HandleCalculationClicked)
        ]
    ];
}

FReply SButtonRowWidget::HandleUploadClicked()
{
    if (OnUploadHeightmap.IsBound())
    {
        OnUploadHeightmap.Execute();
    }
    return FReply::Handled();
}

FReply SButtonRowWidget::HandleAnalyzeHydrologyClicked()
{
    if (OnAnalyzeHydrology.IsBound())
    {
        OnAnalyzeHydrology.Execute();
    }
    return FReply::Handled();
}

FReply SButtonRowWidget::HandleCalculationClicked()
{
    if (OnCalculateBiome.IsBound())
    {
        OnCalculateBiome.Execute();
    }
    return FReply::Handled();
}