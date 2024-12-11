// BiomeEditorToolkit.h
#pragma once

#include "CoreMinimal.h"
#include "ResultsWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "HeightmapCell.h"
#include "BiomeCalculator.h"

class SButtonRowWidget;
class SAltitudeSliderWidget;
class SLatitudeSliderWidget;
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

    // Results Widget for displaying results
    TSharedPtr<SResultsWidget> ResultsWidget;

    // Slider widgets for input
    TSharedPtr<SAltitudeSliderWidget> AltitudeSliderWidget;
    TSharedPtr<SLatitudeSliderWidget> LatitudeSliderWidget;
    TSharedPtr<SMainWidget> MainWidget;

    // Callback functions for button events
    void OnUploadButtonClicked();
    void OnCalculateBiomeClicked();

    // Helper variables for storing slider values
    float MinLatitudeSlider = -90.0f;
    float MaxLatitudeSlider = 90.0f;
    float MinAltitudeSlider = 0.0f;
    float MaxAltitudeSlider = 2000.0f;
    float SeaLevelSlider = 50.0f;

    // Parsed heightmap data
    TArray<FHeightmapCell> HeightmapData;
    float ParsedMinLongitude = 0.0f;
    float ParsedMaxLongitude = 0.0f;
};