#pragma once

#include "CoreMinimal.h"
#include "ResultsWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "HeightmapCell.h"
#include "BiomeCalculator.h"

class SButtonRowWidget;
class SMainWidget;

/**
 * Toolkit for biome editing within the Unreal Editor.
 */

class BIOMEMAPPER_API BiomeEditorToolkit : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeEditorToolkit) {}
    SLATE_END_ARGS()

    /** Constructs the editor toolkit widget. */
    void Construct(const FArguments& InArgs);

private:
    // Biome calculator instance
    UBiomeCalculator* BiomeCalculatorInstance;

    // Heightmap Dimensions
    int32 Width = 0;
    int32 Height = 0;
    FVector2D Resolution;

    // Results Widget for displaying results
    TSharedPtr<SResultsWidget> ResultsWidget;

    // Slider widgets for input   
    TSharedPtr<SMainWidget> MainWidget;

    // Callback functions for button events    
    void OnUploadButtonClicked();
    void OnCalculateBiomeClicked();
    void OnShowHeightmapClicked();
    void OnShowBiomeMapClicked();
    float GetTimeOfYear();

    /** Callback for when parameters are changed in the MainWidget */
    void OnParametersChanged();

    // Texture creation methods
    UTexture2D* CreateHeightmapTexture(const TArray<FHeightmapCell>& MapData, int32 HeightmapWidth, int32 HeightmapHeight);
    UTexture2D* CreateBiomeMapTexture(const TArray<FColor>& TextureData, int32 TextureWidth, int32 TextureHeight);

    // Helper variables for storing slider values
    float DayLength = 24.0f;
    float YearLength = 365.25;
    float DayOfYear = 0.0f;    
    float NorthernLatitudeInput = 35.0f;
    float SouthernLatitudeInput = 25.0f;
    float CentralLongitudeInput = 0.0f;
    float MinimumAltitudeInput = 0.0f;
    float MaximumAltitudeInput = 2000.0f;
    float SeaLevelInput = 250.0f;

    // Parsed heightmap data
    TArray<FHeightmapCell> HeightmapData;
    float ParsedMinLongitude = 0.0f;
    float ParsedMaxLongitude = 0.0f;
};