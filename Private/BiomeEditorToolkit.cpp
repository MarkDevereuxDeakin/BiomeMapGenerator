#include "BiomeEditorToolkit.h"
#include "PlanetTime.h"
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
                SNew(STextBlock).Text(FText::FromString("Day Length (Hours):"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(DayLengthInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Year Length (Days):"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(YearLengthInput, SEditableTextBox)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock).Text(FText::FromString("Months Per Year:"))
            ]
            + SHorizontalBox::Slot()
            [
                SAssignNew(MonthsPerYearInput, SEditableTextBox)
            ]
        ]

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
            TEXT("All Supported Files (*.raw;*.r16;*.r32;*.png;*.jpg;*.jpeg;)|*.raw;*.r16;*.r32;*.png;*.jpg;*.jpeg;|Raw Files (*.raw;*.r16;*.r32)|*.raw;*.r16;*.r32|Image Files (*.png;*.jpg;*.jpeg;)|*.png;*.jpg;*.jpeg;"),
            EFileDialogFlags::None,
            OutFiles))
    {
        if (OutFiles.Num() > 0)
        {
            FString SelectedFile = OutFiles[0];
            HeightmapData.Empty();

            int32 Width, Height;

            // Enhanced error handling and logging
            if (!HeightmapParser::ParseHeightmap(
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
                UE_LOG(LogTemp, Error, TEXT("Failed to parse heightmap: %s"), *SelectedFile);
                UploadStatusText->SetText(FText::FromString(FString::Printf(TEXT("Failed to parse heightmap: %s"), *SelectedFile)));
            }
            else
            {
                UploadStatusText->SetText(FText::FromString(FString::Printf(TEXT("Loaded heightmap (%dx%d)"), Width, Height)));
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

    float DayLength = FCString::Atof(*DayLengthInput->GetText().ToString());
    float YearLength = FCString::Atof(*YearLengthInput->GetText().ToString());
    int32 MonthsPerYear = FCString::Atoi(*MonthsPerYearInput->GetText().ToString());

    // Initialize planetary time
    FPlanetTime PlanetTime(DayLength, YearLength, MonthsPerYear);

    

    FString BiomeResults = BiomeCalculatorInstance->CalculateBiomeFromInput(
        MinLatitude,
        MaxLatitude,
        MinLongitude,
        MaxLongitude,
        MinAltitude,
        MaxAltitude,
        SeaLevel,
        HeightmapData,
        PlanetTime);

    ResultText->SetText(FText::FromString(BiomeResults));
    return FReply::Handled();
}
