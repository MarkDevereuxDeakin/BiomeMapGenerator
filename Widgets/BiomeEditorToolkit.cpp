#include "BiomeEditorToolkit.h"
#include "PlanetTime.h"
#include "MultiHandleSlider.h"
#include "HeaderFooterWidget.h"
#include "MainWidget.h"
#include "AltitudeSliderWidget.h"
#include "LatitudeSliderWidget.h"
#include "ButtonRowWidget.h"
#include "ResultsWidget.h"
#include "SidePaddingWidget.h"
#include "BiomeCalculator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "DesktopPlatformModule.h"
#include "HeightmapParser.h"

void BiomeEditorToolkit::Construct(const FArguments& InArgs)
{
    BiomeCalculatorInstance = NewObject<UBiomeCalculator>();


    ChildSlot
    [
        SNew(SBorder)
        .Padding(10) // Uniform padding around the entire layout
        [
            SNew(SHorizontalBox)

            // Left: Altitude Slider
            + SHorizontalBox::Slot()
            .FillWidth(0.1f)
            [
                SNew(SAltitudeSliderWidget)
                .InitialMinAltitude(MinAltitudeSlider)
                .InitialMaxAltitude(MaxAltitudeSlider)
                .InitialSeaLevel(SeaLevelSlider)
                .OnAltitudeChanged_Lambda([this]() {
                    UE_LOG(LogTemp, Log, TEXT("Altitude slider values updated."));
                })
            ]

            // Center: Main Area
            + SHorizontalBox::Slot()
            .FillWidth(0.8f)
            [
                SNew(SVerticalBox)

                // MainWidget
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SMainWidget)
                    .InitialDayLengthHours(24.0f)
                    .InitialYearLengthDays(365.25f)
                    .OnParametersChanged_Lambda([]() {
                        UE_LOG(LogTemp, Log, TEXT("MainWidget parameters changed."));
                    })
                ]

               // Centralized Buttons (using ButtonRowWidget)
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                .Padding(10)
                [
                    SNew(SButtonRowWidget)
                    .OnUploadHeightmap(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnUploadButtonClicked))
                    .OnCalculateBiome(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnCalculateBiomeClicked))
                ]

                // Results Widget
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(0, 10) // Space between ButtonRow and ResultsWidget
                [
                    SAssignNew(ResultsWidget, SResultsWidget)
                ]
            ]

            // Right: Latitude Slider
            + SHorizontalBox::Slot()
            .FillWidth(0.1f)
            [
                SNew(SLatitudeSliderWidget)
                .InitialMinLatitude(MinLatitudeSlider)
                .InitialMaxLatitude(MaxLatitudeSlider)
                .OnLatitudeChanged_Lambda([this]() {
                    UE_LOG(LogTemp, Log, TEXT("Latitude slider values updated."));
                })
            ]
        ]
    ];
}

void BiomeEditorToolkit::OnUploadButtonClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("Error: Desktop platform not available."));
        }
        return;
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

            if (!HeightmapParser::ParseHeightmap(
                SelectedFile,
                SeaLevelSlider,
                MinAltitudeSlider,
                MaxAltitudeSlider,
                MinLatitudeSlider,
                MaxLatitudeSlider,
                ParsedMinLongitude,
                ParsedMaxLongitude,
                HeightmapData,
                Width,
                Height))
            {
                if (ResultsWidget.IsValid())
                {
                    ResultsWidget->UpdateResults(FString::Printf(TEXT("Failed to parse heightmap: %s"), *SelectedFile));
                }
                UE_LOG(LogTemp, Error, TEXT("Failed to parse heightmap: %s"), *SelectedFile);
            }
            else
            {
                if (ResultsWidget.IsValid())
                {
                    ResultsWidget->UpdateResults(FString::Printf(TEXT("Loaded heightmap (%dx%d)"), Width, Height));
                }
                UE_LOG(LogTemp, Log, TEXT("Successfully loaded heightmap: %s"), *SelectedFile);
            }
        }
    }
    else
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("No file selected."));
        }
    }
}

void BiomeEditorToolkit::OnCalculateBiomeClicked()
{
    if (HeightmapData.Num() == 0)
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("No heightmap data available."));
        }
        return;
    }

    float DayLength = 24.0f; // Example default value
    float YearLength = 365.25f; // Example default value

    FPlanetTime PlanetTime(DayLength, YearLength);

    FString BiomeResults = BiomeCalculatorInstance->CalculateBiomeFromInput(
        MinLatitudeSlider, MaxLatitudeSlider,
        ParsedMinLongitude, ParsedMaxLongitude,
        MinAltitudeSlider, MaxAltitudeSlider,
        SeaLevelSlider, HeightmapData, PlanetTime);

    if (ResultsWidget.IsValid())
    {
        ResultsWidget->UpdateResults(BiomeResults);
    }
}