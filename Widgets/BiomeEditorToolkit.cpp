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
#include "HydrologyCalculator.h"
#include "BiomeCalculator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "DesktopPlatformModule.h"
#include "HeightmapParser.h"
#include "Engine/Texture2D.h"

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
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)

                // MainWidget
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)                
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
                    .OnAnalyzeHydrology(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnAnalyzeHydrologyClicked))
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
            this->HeightmapData.Empty();           

            if (!UHeightmapParser::ParseHeightmap(
                SelectedFile,
                SeaLevelSlider,
                MinAltitudeSlider,
                MaxAltitudeSlider,
                MinLatitudeSlider,
                MaxLatitudeSlider,
                ParsedMinLongitude,
                ParsedMaxLongitude,
                this->HeightmapData,
                this->Width,
                this->Height))
            {
                if (ResultsWidget.IsValid())
                {
                    ResultsWidget->UpdateResults(FString::Printf(TEXT("Failed to parse heightmap: %s"), *SelectedFile));
                }
                UE_LOG(LogTemp, Error, TEXT("Failed to parse heightmap: %s"), *SelectedFile);
            }
            else
            {
                // Create a grayscale texture from HeightmapData
                UTexture2D* HeightmapTexture = CreateHeightmapTexture(HeightmapData, Width, Height);

                if (ResultsWidget.IsValid())
                {
                    ResultsWidget->UpdateHeightmapTexture(HeightmapTexture);
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

UTexture2D* BiomeEditorToolkit::CreateHeightmapTexture(const TArray<FHeightmapCell>& MapData, int32 HeightmapWidth, int32 HeightmapHeight)
{
    // Define the target size
    constexpr int32 MaxTargetSize = 1024;

    // Calculate scale factor to fit within MaxTargetSize
    float ScaleFactor = FMath::Min(MaxTargetSize / static_cast<float>(HeightmapWidth), 
                                   MaxTargetSize / static_cast<float>(HeightmapHeight));
    int32 ScaledWidth = FMath::RoundToInt(HeightmapWidth * ScaleFactor);
    int32 ScaledHeight = FMath::RoundToInt(HeightmapHeight * ScaleFactor);


    TArray<FColor> TextureData;
    TextureData.Reserve(ScaledWidth * ScaledHeight);

    for (int32 y = 0; y < ScaledHeight; ++y)
    {
        for (int32 x = 0; x < ScaledWidth; ++x)
        {
            // Map scaled coordinates back to original data
            float OriginalX = x / ScaleFactor;
            float OriginalY = y / ScaleFactor;

            int32 BaseX = FMath::Clamp(FMath::FloorToInt(OriginalX), 0, HeightmapWidth - 1);
            int32 BaseY = FMath::Clamp(FMath::FloorToInt(OriginalY), 0, HeightmapHeight - 1);

            // Bilinear interpolation between four nearest pixels
            const FHeightmapCell& Cell = MapData[BaseY * HeightmapWidth + BaseX];
            uint8 GrayValue = static_cast<uint8>(FMath::Clamp((Cell.Altitude - MinAltitudeSlider) / (MaxAltitudeSlider - MinAltitudeSlider) * 255.0f, 0.0f, 255.0f));
            TextureData.Add(FColor(GrayValue, GrayValue, GrayValue, 255));
        }
    }

     UTexture2D* HeightmapTexture = UTexture2D::CreateTransient(ScaledWidth, ScaledHeight);
    if (!HeightmapTexture)
    {
        return nullptr;
    }

    if (HeightmapTexture->GetPlatformData() == nullptr)
    {
        HeightmapTexture->SetPlatformData(new FTexturePlatformData());
        HeightmapTexture->GetPlatformData()->SizeX = ScaledWidth;
        HeightmapTexture->GetPlatformData()->SizeY = ScaledHeight;
        HeightmapTexture->GetPlatformData()->PixelFormat = PF_B8G8R8A8;
    }

    FTexture2DMipMap& Mip = HeightmapTexture->GetPlatformData()->Mips[0];
    Mip.BulkData.Lock(LOCK_READ_WRITE);
    void* TextureMemory = Mip.BulkData.Realloc(ScaledWidth * ScaledHeight * sizeof(FColor));
    FMemory::Memcpy(TextureMemory, TextureData.GetData(), TextureData.Num() * sizeof(FColor));
    Mip.BulkData.Unlock();

    HeightmapTexture->UpdateResource();
    return HeightmapTexture;
}

void BiomeEditorToolkit::OnAnalyzeHydrologyClicked()
{
    if (HeightmapData.Num() == 0)
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("No heightmap data available."));
        }
        return;
    }

    // Perform hydrology analysis
    TArray<FVector2D> FlowDirections;
    TArray<float> FlowAccumulation;
    TMap<int32, TArray<FVector>> SeasonalStreamPaths;
    float StreamThreshold = 10.0f; // Example threshold value

    if (HydrologyCalculator::AnalyzeHydrology(
            HeightmapData,
            Width,
            Height,
            FlowDirections,
            FlowAccumulation,
            StreamThreshold,
            SeasonalStreamPaths,
            0.0f))
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("Hydrology analysis complete."));
        }
    }
    else
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("Failed to analyze hydrology."));
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