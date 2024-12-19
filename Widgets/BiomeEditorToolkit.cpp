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
                    .OnAnalyzeHydrology(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnAnalyzeHydrologyClicked))
                    .OnCalculateBiome(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnCalculateBiomeClicked))
                ]

                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(0, 10)
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

void BiomeEditorToolkit::OnAnalyzeHydrologyClicked()
{
    if (HeightmapData.Num() == 0)
    {
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("No heightmap data available."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ResultsWidget is invalid."));
        }
        return;
    }

    // Ensure ResultsWidget validity before updating it
    if (!ResultsWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ResultsWidget is invalid."));
        return;
    }

    // Date Calculation based on user input
    FPlanetTime& PlanetTime = FPlanetTime::GetInstance();

    // Get day of year from planetary time    
    int32 YearLength = PlanetTime.GetYearLength();
    int32 DayOfYear = PlanetTime.GetDayOfYear();

    // Allow configurable StreamThreshold and other parameters
    // Declare necessary output variables
    TArray<FVector2D> OutFlowDirections;     // For flow directions
    TArray<float> OutFlowAccumulation;       // For flow accumulation
    TArray<float> OutFlowDepth; 
    float MaxFlowDepth;
    float StreamThreshold = 100.0f / (Resolution.X + Resolution.Y);
    float TimeOfYear = DayOfYear/YearLength;

    TMap<int32, TArray<FVector>> SeasonalStreamPaths;

    if (!HydrologyCalculator::AnalyzeHydrology(
            HeightmapData,
            this->Width,
            this->Height,
            Resolution,
            OutFlowDirections,
            OutFlowAccumulation,
            OutFlowDepth,
            MaxFlowDepth,
            StreamThreshold,
            SeasonalStreamPaths))
    {
        UE_LOG(LogTemp, Error, TEXT("Hydrology analysis failed."));
        if (ResultsWidget.IsValid())
        {
            ResultsWidget->UpdateResults(TEXT("Failed to analyze hydrology."));
        }
        return;
    }

    TArray<FColor> TextureColors;
    TArray<FVector> TextureVectors;

    // Generate visualization texture
    TArray<FColor> TextureData = HydrologyCalculator::GenerateVisualizationTexture(
        SeasonalStreamPaths, 
        HeightmapData, 
        OutFlowDepth, 
        MaxFlowDepth, 
        Width, Height, 
        MinAltitudeSlider, 
        MaxAltitudeSlider,
        TextureColors,
        TextureVectors);
    
    // Create and display hydrology texture
    UTexture2D* HydrologyTexture = CreateHydrologyTexture(TextureColors, Width, Height);// Where is TextureData coming from

    if (ResultsWidget.IsValid())
    {
        ResultsWidget->UpdateHydrologyVisualization(TextureVectors, Width, Height);// Where is TextureData coming from
    }

    if (!SaveHydrologyImage(TextureColors, Width, Height))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save hydrology image."));
    }
}


UTexture2D* BiomeEditorToolkit::CreateHydrologyTexture(const TArray<FColor>& TextureData, int32 TextureWidth, int32 TextureHeight)
{
    UTexture2D* Texture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_B8G8R8A8);
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create hydrology texture."));
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

bool BiomeEditorToolkit::SaveHydrologyImage(const TArray<FColor>& TextureData, int32 ImageWidth, int32 ImageHeight)
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        UE_LOG(LogTemp, Error, TEXT("Desktop platform not available."));
        return false;
    }

    TArray<FString> OutFiles;
    if (!DesktopPlatform->SaveFileDialog(
            nullptr,
            TEXT("Save Hydrology Image"),
            FPaths::ProjectContentDir(),
            TEXT("Hydrology.png"),
            TEXT("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg"),
            EFileDialogFlags::None,
            OutFiles))
    {
        return false; // User canceled
    }

    FString SavePath = OutFiles[0];

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

    // Check if the module is loaded
    if (!FModuleManager::Get().IsModuleLoaded(TEXT("ImageWrapper")))
    {
        UE_LOG(LogTemp, Error, TEXT("ImageWrapper module is not loaded."));
        return false;
    }

    // Create the image wrapper
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ImageWrapper."));
        return false;
    }

    // Set raw data into the image wrapper
    if (!ImageWrapper->SetRaw(TextureData.GetData(), TextureData.Num() * sizeof(FColor), ImageWidth, ImageHeight, ERGBFormat::BGRA, 8))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set raw image data into ImageWrapper."));
        return false;
    }

    // Compress the data
    const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed();
    if (CompressedData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to compress image data."));
        return false;
    }

    // Save the file
    if (FFileHelper::SaveArrayToFile(CompressedData, *SavePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Hydrology image successfully saved to %s"), *SavePath);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save hydrology image to %s"), *SavePath);
        return false;
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

    FString BiomeResults = BiomeCalculatorInstance->CalculateBiomeFromInput(
        MinLatitudeSlider, MaxLatitudeSlider,
        ParsedMinLongitude, ParsedMaxLongitude,
        MinAltitudeSlider, MaxAltitudeSlider,
        SeaLevelSlider,
        HeightmapData);

    if (ResultsWidget.IsValid())
    {
        ResultsWidget->UpdateResults(BiomeResults);
    }
}

float BiomeEditorToolkit::GetTimeOfYear()
{
    int32 DayOfYear = MainWidget->GetDayOfYear();    
    float DaysInAYear = MainWidget->GetYearLengthDays();

    return DayOfYear / DaysInAYear; // Normalized value between 0.0 and 1.0
}
