#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

class BIOMEMAPPER_API SResultsWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SResultsWidget) {}
    SLATE_END_ARGS()

    /** Constructs the widget */
    void Construct(const FArguments& InArgs);

    /** Updates the displayed results. */
    void UpdateResults(const FString& ResultsText);

     /** Updates the displayed heightmap texture. */
    void UpdateHeightmapTexture(UTexture2D* HeightmapTexture);

    void UpdateHydrologyVisualization(const TArray<FVector>& HydrologyData, int32 Width, int32 Height);

    // Add this function to update the hydrology texture
    void UpdateHydrologyTexture(UTexture2D* HydrologyTexture);   

private:

    // Result Display
    TSharedPtr<STextBlock> ResultsTextBlock;
    TSharedPtr<SWidgetSwitcher> ImageSwitcher; // For switching between images
    TSharedPtr<SImage> HeightmapImage;        // Heightmap display
    TSharedPtr<SImage> HydrologyImage;        // Hydrology display
    TSharedPtr<FSlateImageBrush> HeightmapBrush;   // Brush for heightmap
    TSharedPtr<FSlateImageBrush> HydrologyBrush;

    // Tab switching functions
    FReply OnShowHeightmapClicked();
    FReply OnShowHydrologyClicked();
};