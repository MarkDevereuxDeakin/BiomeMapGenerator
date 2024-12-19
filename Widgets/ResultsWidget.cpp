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
        
        // Tabs for Switching
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        [
            SNew(SHorizontalBox)
            
            // Tab for Heightmap
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("Heightmap"))
                .OnClicked(this, &SResultsWidget::OnShowHeightmapClicked)
            ]

            // Tab for Hydrology
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("Hydrology"))
                .OnClicked(this, &SResultsWidget::OnShowHydrologyClicked)
            ]
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
                    SAssignNew(HydrologyImage, SImage)
                    .Image(HydrologyBrush.Get())
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

void SResultsWidget::UpdateHydrologyVisualization(const TArray<FVector>& HydrologyData, int32 Width, int32 Height)
{
    TArray<FColor> TextureData;
    TextureData.Reserve(Width * Height);

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;
            bool IsHydrologyPoint = HydrologyData.Contains(FVector(x, y, 0.0f));

            FColor PixelColor = IsHydrologyPoint ? FColor::Blue : FColor::Black;
            TextureData.Add(PixelColor);
        }
    }

    UTexture2D* HydrologyTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!HydrologyTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create hydrology texture."));
        return;
    }

    FTexture2DMipMap& Mip = HydrologyTexture->GetPlatformData()->Mips[0];
    Mip.BulkData.Lock(LOCK_READ_WRITE);
    void* TextureMemory = Mip.BulkData.Realloc(Width * Height * sizeof(FColor));
    FMemory::Memcpy(TextureMemory, TextureData.GetData(), TextureData.Num() * sizeof(FColor));
    Mip.BulkData.Unlock();

    HydrologyTexture->UpdateResource();
    UpdateHydrologyTexture(HydrologyTexture);
}

void SResultsWidget::UpdateHydrologyTexture(UTexture2D* HydrologyTexture)
{
    if (!HydrologyTexture)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid hydrology texture passed to UpdateHydrologyTexture."));
        return;
    }

    // Create a new brush if it doesn't exist
    if (!HydrologyBrush.IsValid())
    {
        HydrologyBrush = MakeShareable(new FSlateImageBrush(
            HydrologyTexture,
            FVector2D(HydrologyTexture->GetSizeX(), HydrologyTexture->GetSizeY())
        ));
    }
    else
    {
        // Update the existing brush
        HydrologyBrush->SetResourceObject(HydrologyTexture);
        HydrologyBrush->ImageSize = FVector2D(HydrologyTexture->GetSizeX(), HydrologyTexture->GetSizeY());
    }

    // Update the SImage widget
    if (HydrologyImage.IsValid())
    {
        HydrologyImage->SetImage(HydrologyBrush.Get()); // Use Get() to retrieve the pointer
    }
}


FReply SResultsWidget::OnShowHeightmapClicked()
{
    if (ImageSwitcher.IsValid())
    {
        ImageSwitcher->SetActiveWidgetIndex(0); // Show the Heightmap
    }
    return FReply::Handled();
}

FReply SResultsWidget::OnShowHydrologyClicked()
{
    if (ImageSwitcher.IsValid())
    {
        ImageSwitcher->SetActiveWidgetIndex(1); // Show the Hydrology
    }
    return FReply::Handled();
}


