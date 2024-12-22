#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

class BIOMEMAPPER_API SResultsWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SResultsWidget) {}
    SLATE_END_ARGS()

    /** Constructs the widget */
    void Construct(const FArguments& InArgs, const TArray<FHeightmapCell>& InHeightmapData, int32 InWidth, int32 InHeight);

    /** Updates the displayed results. */
    void UpdateResults(const FString& ResultsText);

     /** Updates the displayed heightmap texture. */
    void UpdateHeightmapTexture(UTexture2D* HeightmapTexture);

    void UpdateBiomeMapTexture(UTexture2D* BiomeMapTexture); 

    // Tab switching functions
    void ShowHeightmap();
    void ShowBiomeMap();

    FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    void UpdateHeightmapData(const TArray<FHeightmapCell>& NewHeightmapData, int32 NewWidth, int32 NewHeight);
    
private:
    TArray<FHeightmapCell> HeightmapData; // Declare HeightmapData here
    int32 Width = 0;
    int32 Height = 0;

    // Result Display
    TSharedPtr<STextBlock> ResultsTextBlock;
    TSharedPtr<SWidgetSwitcher> ImageSwitcher; // For switching between images
    TSharedPtr<SImage> HeightmapImage;        // Heightmap display
    TSharedPtr<SImage> BiomeMapImage;        // Biome Map display
    TSharedPtr<STextBlock> BiomeTypeText;   // Biome Type Text display
    TSharedPtr<FSlateImageBrush> HeightmapBrush;   // Brush for heightmap
    TSharedPtr<FSlateImageBrush> BiomeMapBrush;     // Brush for Biome Map

    
};