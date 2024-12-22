#include "BiomeEditorToolkit.h"
#include "PlanetTime.h"
#include "MultiHandleSlider.h"
#include "HeaderFooterWidget.h"
#include "MainWidget.h"
#include "AltitudeSliderWidget.h"
#include "LatitudeSliderWidget.h"
#include "ButtonRowWidget.h"
#include "TabRowWidget.h"
#include "ResultsWidget.h"
#include "BiomeCalculator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "DesktopPlatformModule.h"
#include "HeightmapParser.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"

void BiomeEditorToolkit::Construct(const FArguments& InArgs)
{
    BiomeCalculatorInstance = NewObject<UBiomeCalculator>();

    ChildSlot
    [
        SNew(SBorder)
        .Padding(10)
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

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                [
                    SNew(SMainWidget)
                    .InitialDayLengthHours(24.0f)
                    .InitialYearLengthDays(365.25f)
                    .InitialDayOfYear(0.0f)
                    .OnParametersChanged_Lambda([]() {
                        UE_LOG(LogTemp, Log, TEXT("MainWidget parameters changed."));
                    })
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                .Padding(10)
                [
                    SNew(SButtonRowWidget)
                    .OnUploadHeightmap(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnUploadButtonClicked))
                    .OnCalculateBiome(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnCalculateBiomeClicked))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                .Padding(10)
                [
                    SNew(STabRowWidget)                    
                    .OnShowHeightmap(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnShowHeightmapClicked))
                    .OnShowBiomeMap(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnShowBiomeMapClicked))
                ]

                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(0, 10)
                [
                    SAssignNew(ResultsWidget, SResultsWidget, HeightmapData, Width, Height)
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
    // Initialize PlanetTime with user inputs
    float DayLengthHours = 24.0f; // Default or fetch from user input
    float YearLength = 365.0f;    // Default or fetch from user input

    // Initialize the PlanetTime singleton
    FPlanetTime::Initialize(YearLength, DayLengthHours, 0.0f, 0, 0.0f);
    
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
            TEXT("All Supported Files (*.raw;*.r16;*.r32;*.png;*.jpg;*.jpeg)|*.raw;*.r16;*.r32;*.png;*.jpg;*.jpeg|Raw Files (*.raw;*.r16;*.r32)|*.raw;*.r16;*.r32|Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg"),
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
                this->Height,
                Resolution))
            {
                if (ResultsWidget.IsValid())
                {
                    ResultsWidget->UpdateResults(FString::Printf(TEXT("Failed to parse heightmap: %s"), *SelectedFile));
                }
                UE_LOG(LogTemp, Error, TEXT("Failed to parse heightmap: %s"), *SelectedFile);
            }
            else
            {
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

    FString BiomeResults = BiomeCalculatorInstance->CalculateBiomeFromInput(
        MinLatitudeSlider, MaxLatitudeSlider,
        ParsedMinLongitude, ParsedMaxLongitude,
        MinAltitudeSlider, MaxAltitudeSlider,
        SeaLevelSlider,
        HeightmapData);

    if (ResultsWidget.IsValid())
    {
        // Pass updated HeightmapData
        ResultsWidget->UpdateHeightmapData(HeightmapData, Width, Height);  
        ResultsWidget->UpdateResults(BiomeResults);       
        
    }   

    TArray<FColor> TextureData;
    TextureData.Reserve(Width * Height);

    for (const FHeightmapCell& Cell : HeightmapData)
    {
        TextureData.Add(Cell.BiomeColor);
    }

    UTexture2D* BiomeMapTexture = CreateBiomeMapTexture(TextureData, Width, Height);

    if (ResultsWidget.IsValid())
    {
        ResultsWidget->UpdateBiomeMapTexture(BiomeMapTexture);
    }
}

float BiomeEditorToolkit::GetTimeOfYear()
{
    int32 DayOfYear = MainWidget->GetDayOfYear();    
    float DaysInAYear = MainWidget->GetYearLengthDays();

    return DayOfYear / DaysInAYear; // Normalized value between 0.0 and 1.0
}

UTexture2D* BiomeEditorToolkit::CreateBiomeMapTexture(const TArray<FColor>& TextureData, int32 TextureWidth, int32 TextureHeight)
{
    UTexture2D* Texture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_B8G8R8A8);
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create biome map texture."));
        return nullptr;
    }

    FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
    Mip.BulkData.Lock(LOCK_READ_WRITE);
    void* Data = Mip.BulkData.Realloc(TextureWidth * TextureHeight * sizeof(FColor));
    FMemory::Memcpy(Data, TextureData.GetData(), TextureData.Num() * sizeof(FColor));
    Mip.BulkData.Unlock();

    Texture->UpdateResource();
    return Texture;
}

void BiomeEditorToolkit::OnShowHeightmapClicked()
{
    if (ResultsWidget.IsValid())
    {
        ResultsWidget->ShowHeightmap();
    }
}

void BiomeEditorToolkit::OnShowBiomeMapClicked()
{
    if (ResultsWidget.IsValid())
    {
        ResultsWidget->ShowBiomeMap();
    }
}
