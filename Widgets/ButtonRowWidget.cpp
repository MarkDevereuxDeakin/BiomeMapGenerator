// ButtonRowWidget.cpp
#include "ButtonRowWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"


void SButtonRowWidget::Construct(const FArguments& InArgs)
{
    OnUploadHeightmap = InArgs._OnUploadHeightmap;
    OnCalculateBiome = InArgs._OnCalculateBiome;

    ChildSlot
    [
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        [
            SNew(SButton)
            .Text(FText::FromString("Upload Heightmap"))
            .OnClicked(this, &SButtonRowWidget::HandleUploadClicked)
        ]

        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
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

FReply SButtonRowWidget::HandleCalculationClicked()
{
    if (OnCalculateBiome.IsBound())
    {
        OnCalculateBiome.Execute();
    }
    return FReply::Handled();
}