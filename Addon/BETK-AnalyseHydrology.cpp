#include "BiomeEditorToolkit.h"
#include "HydrologyCalculator.h"

/*void BiomeEditorToolkit::OnAnalyzeHydrologyClicked()
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
}*/


/*UTexture2D* BiomeEditorToolkit::CreateHydrologyTexture(const TArray<FColor>& TextureData, int32 TextureWidth, int32 TextureHeight)
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

}*/


/**
 * BiomeEditorToolkit.h addon
 * 
 * 
    
    UTexture2D* CreateHydrologyTexture(const TArray<FColor>& TextureData, int32 Width, int32 Height);
    // Save hydrology image
    bool SaveHydrologyImage(const TArray<FColor>& TextureData, int32 Width, int32 Height);

    // Callback for hydrology calculation
    void OnHydrologyCalculationComplete(const TArray<FVector>& HydrologyData, int32 Width, int32 Height);
 */