#include "BiomeEditorToolkit.h"
#include "PlanetTime.h"
#include "BiomeInputShared.h"
#include "HeaderFooterWidget.h"
#include "MainWidget.h"
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

FInputParameters InputParams;

void BiomeEditorToolkit::Construct(const FArguments& InArgs)
{
    BiomeCalculatorInstance = NewObject<UBiomeCalculator>();

    // Initialize MainWidget properly
    SAssignNew(MainWidget, SMainWidget)
        .OnParametersChanged(FSimpleDelegate::CreateRaw(this, &BiomeEditorToolkit::OnParametersChanged));

    ChildSlot
    [
        SNew(SBorder)
        .Padding(10)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
            .FillWidth(0.2f)
            [
                SNew(SHorizontalBox)            

                // Center: Main Area
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    [
                         MainWidget.ToSharedRef() // Use MainWidget
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
                ]
            ]

            + SHorizontalBox::Slot()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
            .FillWidth(0.8f)
            [
                SNew (SVerticalBox)                
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
           
        ]
    ];
}

void BiomeEditorToolkit::OnUploadButtonClicked()
{
    // Initialize PlanetTime with user inputs
    float DayLengthHours = DayLength; // Default or fetch from user input
    float YearDuration = YearLength;    // Default or fetch from user input
    int32 Date = DayOfYear;

    // Populate InputParams with current values
    InputParams.NorthernLatitude = MainWidget->GetNorthernLatitude();
    InputParams.SouthernLatitude = MainWidget->GetSouthernLatitude();
    InputParams.MaximumAltitude = MainWidget->GetMaximumAltitude();
    InputParams.MinimumAltitude = MainWidget->GetMinimumAltitude();
    InputParams.SeaLevel = MainWidget->GetSeaLevel();

    // Initialize the PlanetTime singleton
    FPlanetTime::Initialize(YearDuration, DayLength, 0.0f, Date, 0.0f);
    
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
                InputParams,
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
            uint8 GrayValue = static_cast<uint8>(FMath::Clamp((Cell.Altitude - InputParams.MinimumAltitude) / (InputParams.MaximumAltitude - InputParams.MinimumAltitude) * 255.0f, 0.0f, 255.0f));
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
        InputParams,
        ParsedMinLongitude,
        ParsedMaxLongitude,
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
    int32 Day = MainWidget->GetDayOfYear();    
    float Year = MainWidget->GetYearLengthDays();

    return Day / Year; // Normalized value between 0.0 and 1.0
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

void BiomeEditorToolkit::OnParametersChanged()
{
    if (MainWidget.IsValid())
    {
        InputParams.NorthernLatitude = MainWidget->GetNorthernLatitude();
        InputParams.SouthernLatitude = MainWidget->GetSouthernLatitude();
        InputParams.MaximumAltitude = MainWidget->GetMaximumAltitude();
        InputParams.MinimumAltitude = MainWidget->GetMinimumAltitude();
        InputParams.SeaLevel = MainWidget->GetSeaLevel();

        UE_LOG(LogTemp, Log, TEXT("Parameters Changed:"));
        UE_LOG(LogTemp, Log, TEXT("Northern Latitude: %.2f"), InputParams.NorthernLatitude);
        UE_LOG(LogTemp, Log, TEXT("Southern Latitude: %.2f"), InputParams.SouthernLatitude);
        UE_LOG(LogTemp, Log, TEXT("Maximum Altitude: %.2f"), InputParams.MaximumAltitude);
        UE_LOG(LogTemp, Log, TEXT("Minimum Altitude: %.2f"), InputParams.MinimumAltitude);
        UE_LOG(LogTemp, Log, TEXT("Sea Level: %.2f"), InputParams.SeaLevel);
    }
}