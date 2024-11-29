#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "HeightmapCell.h"

class SEditableTextBox;
class STextBlock;

/**
 * Toolkit for biome editing within the Unreal Editor.
 */
class BIOMEMAPPER_API BiomeEditorToolkit : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeEditorToolkit) {}
    SLATE_END_ARGS()

    /**
     * Construct the editor toolkit widget.
     * @param InArgs - Slate arguments.
     */
    void Construct(const FArguments& InArgs);

private:
    // UI Elements for user input
    TSharedPtr<SEditableTextBox> DayLengthInput;
    TSharedPtr<SEditableTextBox> YearLengthInput;
    TSharedPtr<SEditableTextBox> MonthsPerYearInput;
    TSharedPtr<SEditableTextBox> MinLatitudeInput;
    TSharedPtr<SEditableTextBox> MaxLatitudeInput;
    TSharedPtr<SEditableTextBox> MinLongitudeInput;
    TSharedPtr<SEditableTextBox> MaxLongitudeInput;
    TSharedPtr<SEditableTextBox> MinAltitudeInput;
    TSharedPtr<SEditableTextBox> MaxAltitudeInput;
    TSharedPtr<SEditableTextBox> SeaLevelInput;

    // UI Text Blocks
    TSharedPtr<STextBlock> UploadStatusText;
    TSharedPtr<STextBlock> ResultText;

    // Parsed heightmap data
    TArray<FHeightmapCell> HeightmapData;

    /**
     * Handler for the Upload button click event.
     * @return Reply indicating the button click was handled.
     */
    FReply OnUploadButtonClicked();

    /**
     * Handler for the Calculate Biome button click event.
     * @return Reply indicating the button click was handled.
     */
    FReply OnCalculateBiomeClicked();
};
