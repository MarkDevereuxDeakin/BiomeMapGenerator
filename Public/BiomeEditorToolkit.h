#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "HeightmapCell.h"

class SEditableTextBox;
class STextBlock;

class BIOMEMAPPER_API BiomeEditorToolkit : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeEditorToolkit) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    // UI Elements
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
    TSharedPtr<STextBlock> UploadStatusText;
    TSharedPtr<STextBlock> ResultText;

    // Parsed Heightmap Data
    TArray<FHeightmapCell> HeightmapData;

    // Event Handlers
    FReply OnUploadButtonClicked();
    FReply OnCalculateBiomeClicked();
};
