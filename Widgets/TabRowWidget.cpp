// TabRowWidget.cpp
#include "TabRowWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"


void STabRowWidget::Construct(const FArguments& InArgs)
{
    OnShowHeightmap = InArgs._OnShowHeightmap; // Bind delegates
    OnShowBiomeMap = InArgs._OnShowBiomeMap; // Bind delegates

    ChildSlot
    [
        SNew(SHorizontalBox)
            
        // Tab for Heightmap
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(150.0f)
            [
                SNew(SButton)
                .Text(FText::FromString("Show Heightmap"))
                .OnClicked(this, &STabRowWidget::HandleHeightmapClicked)
            ]
        ]

        // Tab for Biome Map
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(150.0f)
            [
                SNew(SButton)
                .Text(FText::FromString("Show Biome Map"))
                .OnClicked(this, &STabRowWidget::HandleBiomeMapClicked)
            ]
            
        ]
    ];
}

FReply STabRowWidget::HandleHeightmapClicked()
{
    if (OnShowHeightmap.IsBound())
    {
        OnShowHeightmap.Execute();
    }
    return FReply::Handled();
}

FReply STabRowWidget::HandleBiomeMapClicked()
{
    if (OnShowBiomeMap.IsBound())
    {
        OnShowBiomeMap.Execute();
    }
    return FReply::Handled();
}