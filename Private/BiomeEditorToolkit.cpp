#include "BiomeEditorToolkit.h"
#include "BiomeCalculator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "DesktopPlatformModule.h"
#include "HeightmapParser.h"

UBiomeCalculator* BiomeCalculatorInstance = NewObject<UBiomeCalculator>();

void BiomeEditorToolkit::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)

        // Title
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString("Biome Editor Toolkit"))
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
        ]

        // Input Fields
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Min Latitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MinLatitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Max Latitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MaxLatitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Min Longitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MinLongitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Max Longitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MaxLongitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Min Altitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MinAltitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Max Altitude:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MaxAltitudeInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Sea Level:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(SeaLevelInput, SEditableTextBox)
            ]
        ]        

        // Upload Button
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SButton)
            .Text(FText::FromString("Upload Heightmap"))
            .OnClicked(this, &BiomeEditorToolkit::OnUploadButtonClicked)
        ]

        // Status Display
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(UploadStatusText, STextBlock)
            .Text(FText::FromString("No file uploaded."))
        ]

        // Calculate Button
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SButton)
            .Text(FText::FromString("Calculate Biome"))
            .OnClicked(this, &BiomeEditorToolkit::OnCalculateBiomeClicked)
        ]

        // Result Display
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(ResultText, STextBlock)
            .Text(FText::FromString("Biome results will appear here."))
        ]
    ];
}

FReply BiomeEditorToolkit::OnUploadButtonClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        UploadStatusText->SetText(FText::FromString("Error: Desktop platform not available."));
        return FReply::Handled();
    }

    TArray<FString> OutFiles;
    if (DesktopPlatform->OpenFileDialog(
            nullptr, 
            TEXT("Select Heightmap"), 
            FPaths::ProjectContentDir(), 
            TEXT(""), 
            TEXT("Heightmap Files (*.raw;*.r16;*.r32)|*.raw;*.r16;*.r32"), 
            EFileDialogFlags::None, 
            OutFiles))
    {
        if (OutFiles.Num() > 0)
        {
            FString SelectedFile = OutFiles[0];
            HeightmapData.Empty();

            int32 Width, Height;
            if (ParseHeightmap(
                SelectedFile,
                FCString::Atof(*SeaLevelInput->GetText().ToString()),
                FCString::Atof(*MinAltitudeInput->GetText().ToString()),
                FCString::Atof(*MaxAltitudeInput->GetText().ToString()),
                FCString::Atof(*MinLatitudeInput->GetText().ToString()),
                FCString::Atof(*MaxLatitudeInput->GetText().ToString()),
                FCString::Atof(*MinLongitudeInput->GetText().ToString()),
                FCString::Atof(*MaxLongitudeInput->GetText().ToString()),
                HeightmapData,
                Width,
                Height))
            {
                UploadStatusText->SetText(FText::FromString(FString::Printf(TEXT("Loaded heightmap (%dx%d)"), Width, Height)));
            }
            else
            {
                UploadStatusText->SetText(FText::FromString("Failed to parse heightmap."));
            }
        }
    }
    else
    {
        UploadStatusText->SetText(FText::FromString("No file selected."));
    }

    return FReply::Handled();
}

FReply BiomeEditorToolkit::OnCalculateBiomeClicked()
{
     if (HeightmapData.Num() == 0)
    {
        UploadStatusText->SetText(FText::FromString("No heightmap data available."));
        return FReply::Handled();
    }

    FString MinLatitude = MinLatitudeInput->GetText().ToString();
    FString MaxLatitude = MaxLatitudeInput->GetText().ToString();
    FString MinLongitude = MinLongitudeInput->GetText().ToString();
    FString MaxLongitude = MaxLongitudeInput->GetText().ToString();
    FString MinAltitude = MinAltitudeInput->GetText().ToString();
    FString MaxAltitude = MaxAltitudeInput->GetText().ToString();
    FString SeaLevel = SeaLevelInput->GetText().ToString();

    //UBiomeCalculator* BiomeCalculatorInstance = NewObject<UBiomeCalculator>();

    FString BiomeResults = BiomeCalculatorInstance->CalculateBiomeFromInput(
        MinLatitude,
        MaxLatitude,
        MinLongitude,
        MaxLongitude,
        MinAltitude,
        MaxAltitude,
        SeaLevel,
        HeightmapData);

    ResultText->SetText(FText::FromString(BiomeResults));
    return FReply::Handled();
}
