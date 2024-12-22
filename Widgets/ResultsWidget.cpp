#include "ResultsWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

void SResultsWidget::Construct(const FArguments& InArgs, const TArray<FHeightmapCell>& InHeightmapData, int32 InWidth, int32 InHeight)
{
    HeightmapData = InHeightmapData;
    Width = InWidth;
    Height = InHeight;
    
    ChildSlot
    [
        SNew(SVerticalBox)

        // Biome Type Display
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .Padding(5)
        [
            SAssignNew(BiomeTypeText, STextBlock)
            .Text(FText::FromString("Hover over a biome to see its type"))
            .Justification(ETextJustify::Center)
        ]

        // Widget Switcher for Shared Display Space
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .Padding(5)
        [
            SAssignNew(ImageSwitcher, SWidgetSwitcher)

            // Heightmap Image
            + SWidgetSwitcher::Slot()
            [
                SNew(SBox)
                .WidthOverride(1024.0f)
                .HeightOverride(1024.0f)
                [
                    SAssignNew(HeightmapImage, SImage)
                    .Image(HeightmapBrush.Get())
                ]
            ]

            // Hydrology Image
            + SWidgetSwitcher::Slot()
            [
                SNew(SBox)
                .WidthOverride(1024.0f)
                .HeightOverride(1024.0f)
                [
                    SAssignNew(BiomeMapImage, SImage)
                    .Image(BiomeMapBrush.Get())
                ]
            ]
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
        UE_LOG(LogTemp, Error, TEXT("HeightmapTexture is null."));
        return;
    }

    if (!HeightmapBrush.IsValid())
    {
        HeightmapBrush = MakeShareable(new FSlateImageBrush(HeightmapTexture,
            FVector2D(HeightmapTexture->GetSizeX(), HeightmapTexture->GetSizeY())));
    }
    else
    {
        HeightmapBrush->SetResourceObject(HeightmapTexture);
        HeightmapBrush->ImageSize = FVector2D(HeightmapTexture->GetSizeX(), HeightmapTexture->GetSizeY());
    }    

    if (HeightmapImage.IsValid())
    {
        HeightmapImage->SetImage(HeightmapBrush.Get());
        UE_LOG(LogTemp, Log, TEXT("Heightmap image updated with dimensions: %dx%d"),
            HeightmapTexture->GetSizeX(), HeightmapTexture->GetSizeY());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HeightmapImage is not valid."));
    }
}

void SResultsWidget::UpdateBiomeMapTexture(UTexture2D* BiomeMapTexture)
{
    if (!BiomeMapTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("BiomeMapTexture is null."));
        return;
    }

    if (!BiomeMapBrush.IsValid())
    {
        BiomeMapBrush = MakeShareable(new FSlateImageBrush(BiomeMapTexture,
            FVector2D(BiomeMapTexture->GetSizeX(), BiomeMapTexture->GetSizeY())));
    }
    else
    {
        BiomeMapBrush->SetResourceObject(BiomeMapTexture);
        BiomeMapBrush->ImageSize = FVector2D(BiomeMapTexture->GetSizeX(), BiomeMapTexture->GetSizeY());
    }

    if (BiomeMapImage.IsValid())
    {
        BiomeMapImage->SetImage(BiomeMapBrush.Get());
        UE_LOG(LogTemp, Log, TEXT("Biome map image updated with dimensions: %dx%d"),
            BiomeMapTexture->GetSizeX(), BiomeMapTexture->GetSizeY());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BiomeMapImage is not valid."));
    }
}

void SResultsWidget::ShowHeightmap()
{
    if (ImageSwitcher.IsValid())
    {
        ImageSwitcher->SetActiveWidgetIndex(0); // Switch to Heightmap
    }
}

void SResultsWidget::ShowBiomeMap()
{
    if (ImageSwitcher.IsValid())
    {
        ImageSwitcher->SetActiveWidgetIndex(1); // Switch to Biome Map
    }
}

FReply SResultsWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

    // Get the size of the BiomeMap texture display area
    FVector2D TextureDisplaySize = MyGeometry.GetLocalSize();

    // Ensure the cursor is within the bounds of the BiomeMap
    if (LocalMousePosition.X < 0.0f || LocalMousePosition.Y < 0.0f || 
        LocalMousePosition.X >= TextureDisplaySize.X || 
        LocalMousePosition.Y >= TextureDisplaySize.Y)
    {
        // Reset biome text if the cursor is outside the bounds
        if (BiomeTypeText.IsValid())
        {
            BiomeTypeText->SetText(FText::FromString(TEXT("")));
        }
        return FReply::Handled();
    }

    // Map cursor position to BiomeMap grid dimensions
    int32 GridX = FMath::Clamp(FMath::FloorToInt((LocalMousePosition.X / TextureDisplaySize.X) * Width), 0, Width - 1);
    int32 GridY = FMath::Clamp(FMath::FloorToInt((LocalMousePosition.Y / TextureDisplaySize.Y) * Height), 0, Height - 1);

    
        int32 Index = GridY * Width + GridX;
        if (HeightmapData.IsValidIndex(Index))
        {
            const FHeightmapCell& Cell = HeightmapData[Index];
            if (BiomeTypeText.IsValid())
            {
                BiomeTypeText->SetText(FText::FromString(Cell.BiomeType));
            }
        }    

    return FReply::Handled();
}

void SResultsWidget::UpdateHeightmapData(const TArray<FHeightmapCell>& NewHeightmapData, int32 NewWidth, int32 NewHeight)
{
    HeightmapData = NewHeightmapData;
    Width = NewWidth;
    Height = NewHeight;
}
