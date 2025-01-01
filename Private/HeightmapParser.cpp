#include "HeightmapParser.h"
#include "Async/ParallelFor.h"
#include "BiomeInputShared.h"
#include "Altitude.h"
#include "Preprocessing.h"
#include "DistanceToOcean.h"
#include "OceanTemperature.h"
#include "Precipitation.h"
#include "UnifiedWindCalculator.h"
#include "Misc/FileHelper.h"
#include "Math/UnrealMathUtility.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

bool UHeightmapParser::ParseHeightmap(
    const FString& FilePath,
    FInputParameters& InputParams,
    float& OutMinLongitude,
    float& OutMaxLongitude,
    TArray<FHeightmapCell>& OutHeightmapData,
    int32& OutWidth,
    int32& OutHeight,
    FVector2D& OutResolution)
{
    TArray<float> RawData;
    int32 BitDepth = 0;

    if (!LoadHeightmap(FilePath, RawData, OutWidth, OutHeight, BitDepth))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap: %s"), *FilePath);
        return false;
    }

    if (OutWidth <= 0 || OutHeight <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid heightmap dimensions: %dx%d"), OutWidth, OutHeight);
        return false;
    }

    if (OutWidth * OutHeight != RawData.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Mismatch between heightmap dimensions and data size. Width: %d, Height: %d, RawData size: %d"), OutWidth, OutHeight, RawData.Num());
        return false;
    }

    EstimateLongitudeRange(
        InputParams.SouthernLatitude, 
        InputParams.NorthernLatitude, 
        OutWidth, 
        OutHeight, 
        InputParams.CentralLongitude,
        OutMinLongitude, 
        OutMaxLongitude);

    // Calculate resolution (pixels per degree)
    float LatitudeRange = InputParams.NorthernLatitude - InputParams.SouthernLatitude;
    float LongitudeRange = OutMaxLongitude - OutMinLongitude;

    OutResolution.X = OutHeight / LatitudeRange; // Pixels per degree latitude
    OutResolution.Y = OutWidth / LongitudeRange; // Pixels per degree longitude

    // Log resolution for debugging
    UE_LOG(LogTemp, Log, TEXT("Heightmap resolution: %f px/degree (latitude), %f px/degree (longitude)"), OutResolution.X, OutResolution.Y);

    // Parse raw data into heightmap cells
    OutHeightmapData.Empty(OutWidth * OutHeight);
    for (int32 y = 0; y < OutHeight; ++y)
    {
        for (int32 x = 0; x < OutWidth; ++x)
        {
            int32 Index = y * OutWidth + x;

             if (Index >= RawData.Num())
            {
                UE_LOG(LogTemp, Error, TEXT("Index out of bounds: %d, RawData size: %d"), Index, RawData.Num());
                return false;
            }

             // Normalize RawData value
            float NormalizedValue = FMath::Clamp(RawData[Index], 0.0f, 1.0f);

            // Convert normalized value to pixel value
            uint8 PixelValue = static_cast<uint8>(NormalizedValue * 255.0f);

            FHeightmapCell Cell;
            Cell.Latitude = InputParams.SouthernLatitude + 
                            (InputParams.NorthernLatitude - InputParams.SouthernLatitude) * 
                            (y / static_cast<float>(OutHeight));
            Cell.Longitude = OutMinLongitude + 
                            (OutMaxLongitude - OutMinLongitude) * 
                            (x / static_cast<float>(OutWidth));
            Cell.Altitude = CalculateAltitude(PixelValue, InputParams.MinimumAltitude, InputParams.MaximumAltitude);

            if (Cell.Altitude <= InputParams.SeaLevel)
            {
                Cell.OceanDepth = CalculateOceanDepth(InputParams.SeaLevel, Cell.Altitude);
                Cell.DistanceToOcean = 0.0f;
                Cell.CellType = ECellType::Ocean;

                // Assign ocean temperature based on latitude and current type
                float BaseOceanTemperature = FMath::Clamp(30.0f - FMath::Abs(Cell.Latitude) * 0.5f, -2.0f, 30.0f);
                FString CurrentType = OceanCurrents::DetermineOceanCurrentType(Cell.Latitude, Cell.Longitude, Cell.FlowDirection);
                Cell.ClosestOceanTemperature = (CurrentType == "warm") ? BaseOceanTemperature + 7.5f : BaseOceanTemperature - 7.5f;

                // Determine the Ocean Current Flow Direction
                Cell.FlowDirection = OceanCurrents::ValidateFlowDirection(Cell.Latitude, Cell.Longitude, Cell.FlowDirection);
            }
            else
            {
                Cell.OceanDepth = 0.0f;
                Cell.CellType = ECellType::Land;                
            }

            OutHeightmapData.Add(Cell);
        }
    }

    // DistanceToOcean calculation
    if (!CalculateDistanceToOcean(OutHeightmapData, OutWidth, OutHeight))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to calculate distances to ocean."));
        return false;
    }

    // Preprocess additional derived data
    Preprocessing::PreprocessData(OutHeightmapData, OutWidth, OutHeight);

    return true;
}

bool UHeightmapParser::LoadHeightmap(
    const FString& FilePath,
    TArray<float>& OutRawData,
    int32& OutWidth,
    int32& OutHeight,
    int32& BitDepth)
{
    FString FileExtension = FPaths::GetExtension(FilePath).ToLower();

    if (FileExtension == TEXT("png") || FileExtension == TEXT("jpg") || FileExtension == TEXT("jpeg"))
    {
        return ParseImageHeightmap(FilePath, OutRawData, OutWidth, OutHeight, BitDepth);
    }
    else if (FileExtension == TEXT("r16") || FileExtension == TEXT("r32") || FileExtension == TEXT("raw"))
    {
        BitDepth = (FileExtension == TEXT("r16")) ? 16 : 32;
        return ParseRawHeightmap(FilePath, OutRawData, OutWidth, OutHeight, BitDepth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported file format: %s"), *FileExtension);
        return false;
    }
}

bool UHeightmapParser::ParseImageHeightmap(
    const FString& FilePath,
    TArray<float>& OutRawData,
    int32& OutWidth,
    int32& OutHeight,
    int32& BitDepth)
{
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load image file: %s"), *FilePath);
        return false;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper for file: %s"), *FilePath);
        return false;
    }

    if (!ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid or unsupported image format: %s"), *FilePath);
        return false;
    }

    OutWidth = ImageWrapper->GetWidth();
    OutHeight = ImageWrapper->GetHeight();
    BitDepth = ImageWrapper->GetBitDepth();
    if (BitDepth == 32)
    {
        TArray<uint8> RawByteData;
        if (!ImageWrapper->GetRaw(ERGBFormat::Gray, 32, RawByteData))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to decompress 32-bit image: %s"), *FilePath);
            return false;
        }

        // Convert raw byte data directly to normalized floating-point values
        int32 NumPixels = RawByteData.Num() / 4; // Each pixel is 4 bytes
        OutRawData.Reserve(NumPixels);

        for (int32 i = 0; i < RawByteData.Num(); i += 4)
        {
            uint32 Value = (RawByteData[i + 3] << 24) | (RawByteData[i + 2] << 16) |
                        (RawByteData[i + 1] << 8) | RawByteData[i]; // Combine bytes into a 32-bit integer

            // Normalize to [0.0, 1.0] using the maximum possible value for 32-bit data
            OutRawData.Add(static_cast<float>(Value) / 4294967295.0f);
        }
    }
    else if (BitDepth == 16)
    {
        TArray<uint8> RawByteData;
        if (!ImageWrapper->GetRaw(ERGBFormat::Gray, 16, RawByteData))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to decompress 16-bit image: %s"), *FilePath);
            return false;
        }

        // Convert raw byte data directly to normalized floating-point values
        int32 NumPixels = RawByteData.Num() / 2; // Each pixel is 2 bytes
        OutRawData.Reserve(NumPixels);

        for (int32 i = 0; i < RawByteData.Num(); i += 2)
        {
            uint16 Value = (RawByteData[i + 1] << 8) | RawByteData[i]; // Combine bytes into a 16-bit integer
            OutRawData.Add(static_cast<float>(Value) / 65535.0f); // Normalize to [0.0, 1.0] and store as float
        }
    }
    else if (BitDepth == 8)
    {
        TArray<uint8> Raw8BitData;
        if (!ImageWrapper->GetRaw(ERGBFormat::Gray, 8, Raw8BitData))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to decompress 8-bit image: %s"), *FilePath);
            return false;
        }

        OutRawData.Reserve(Raw8BitData.Num());
        for (uint8 Value : Raw8BitData)
        {
            OutRawData.Add(Value / 255.0f); // Normalize to [0.0, 1.0]
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported bit depth: %d in file: %s"), BitDepth, *FilePath);
        return false;
    }

    return true;
}

bool UHeightmapParser::ParseRawHeightmap(
    const FString& FilePath,
    TArray<float>& OutHeightmapData,
    int32& OutWidth,
    int32& OutHeight,
    int32 BitDepth)
{
    TArray<uint8> RawData;
   if (!FFileHelper::LoadFileToArray(RawData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load raw heightmap file: %s"), *FilePath);
        return false;
    }

    if (IsBigEndian(RawData, BitDepth))
    {
        if (BitDepth == 16)
        {
            ConvertToLittleEndian(RawData);
        }
        else if (BitDepth == 32)
        {
            ConvertToLittleEndian32(RawData);
        }
    }

    int64 FileSize = RawData.Num();

    // Step 1: Attempt to read metadata
    FString MetadataFilePath = FPaths::ChangeExtension(FilePath, TEXT("hdr"));
    if (FPaths::FileExists(MetadataFilePath))
    {
        if (ReadMetadataFromFile(MetadataFilePath, OutWidth, OutHeight, BitDepth))
        {
            UE_LOG(LogTemp, Log, TEXT("Metadata file used to determine dimensions: %dx%d"), OutWidth, OutHeight);
        }
    }

    // Step 2: Infer dimensions from file size if metadata is not available
    if (OutWidth == 0 || OutHeight == 0)
    {
        if (!DetermineDimensionsFromFile(FileSize, BitDepth, OutWidth, OutHeight))
        {
            UE_LOG(LogTemp, Warning, TEXT("Determining dimensions failed, attempting to guess dimensions."));

            // Step 3: Guess dimensions as a fallback
            if (!GuessDimensions(FileSize, BitDepth, OutWidth, OutHeight))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to determine dimensions for file: %s"), *FilePath);
                return false;
            }
        }
    }

    // Step 4: Parse the raw data into floating-point values
    int32 BytesPerPixel = BitDepth / 8;
    int32 NumSamples = FileSize / BytesPerPixel;
    OutHeightmapData.Reserve(NumSamples);

    if (BitDepth == 16)
    {
        for (int32 i = 0; i < RawData.Num(); i += 2)
        {
            uint16 Value = (RawData[i + 1] << 8) | RawData[i];
            OutHeightmapData.Add(Value / 65535.0f); // Normalize to [0.0, 1.0]
        }
    }
    else if (BitDepth == 32)
    {
        for (int32 i = 0; i < RawData.Num(); i += 4)
        {
            uint32 Value = (RawData[i + 3] << 24) | (RawData[i + 2] << 16) | (RawData[i + 1] << 8) | RawData[i];
            OutHeightmapData.Add(*reinterpret_cast<float*>(&Value));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported bit depth: %d"), BitDepth);
        return false;
    }

    return true;
}

/***
 * This is the Estimate Longitude for this file. The new current version
 */
 void UHeightmapParser::EstimateLongitudeRange(
    float MinLatitude,
    float MaxLatitude,
    int32 Width,
    int32 Height,
    float CentralLongitude,
    float& OutMinLongitude,
    float& OutMaxLongitude)
{
    float AspectRatio = Width / static_cast<float>(Height);
    float AverageLatitude = (MinLatitude + MaxLatitude) / 2.0f;
    float HalfLongitudeRange = (AspectRatio * 180.0f / FMath::Cos(FMath::DegreesToRadians(AverageLatitude));)

    OutMinLongitude = CentralLongitude - HalfLongitudeRange;
    OutMaxLongitude = CentralLongitude + HalfLongitudeRange;
}

bool UHeightmapParser::IsBigEndian(const TArray<uint8>& Data, int32 BitDepth)
{
    if (BitDepth == 16)
    {
        uint16 Value = *(reinterpret_cast<const uint16*>(Data.GetData()));
        return Value > 0xFF;
    }
    else if (BitDepth == 32)
    {
        uint32 Value = *(reinterpret_cast<const uint32*>(Data.GetData()));
        return Value > 0xFFFFFF;
    }
    return false;
}

void UHeightmapParser::ConvertToLittleEndian(TArray<uint8>& Data)
{
    int32 NumValues = Data.Num() / 2;
    ParallelFor(NumValues, [&Data](int32 Index)
    {
        uint8 Temp = Data[Index * 2];
        Data[Index * 2] = Data[Index * 2 + 1];
        Data[Index * 2 + 1] = Temp;
    });
}

void UHeightmapParser::ConvertToLittleEndian32(TArray<uint8>& Data)
{
    int32 NumValues = Data.Num() / 4;
    ParallelFor(NumValues, [&Data](int32 Index)
    {
        uint8 Temp1 = Data[Index * 4];
        uint8 Temp2 = Data[Index * 4 + 1];
        Data[Index * 4] = Data[Index * 4 + 3];
        Data[Index * 4 + 1] = Data[Index * 4 + 2];
        Data[Index * 4 + 2] = Temp2;
        Data[Index * 4 + 3] = Temp1;
    });
}

bool UHeightmapParser::DetermineDimensionsFromFile(int64 FileSize, int32 BitDepth, int32& OutWidth, int32& OutHeight)
{
    int32 BytesPerPixel = BitDepth / 8;
    int32 NumSamples = FileSize / BytesPerPixel;

    // Try square dimensions first
    OutWidth = FMath::Sqrt(static_cast<float>(NumSamples));
    OutHeight = NumSamples / OutWidth;

    if (OutWidth * OutHeight == NumSamples)
    {
        return true; // Dimensions successfully determined
    }

    UE_LOG(LogTemp, Warning, TEXT("Failed to determine dimensions. File size: %lld bytes, Bit depth: %d, Samples: %d"),
           FileSize, BitDepth, NumSamples);

    return false;
}


bool UHeightmapParser::ReadMetadataFromFile(const FString& MetadataFilePath, int32& OutWidth, int32& OutHeight, int32& OutBitDepth)
{
    FString MetadataContent;
    if (FFileHelper::LoadFileToString(MetadataContent, *MetadataFilePath))
    {
        FRegexPattern WidthPattern(TEXT("Width:\\s*(\\d+)"));
        FRegexPattern HeightPattern(TEXT("Height:\\s*(\\d+)"));
        FRegexPattern BitDepthPattern(TEXT("BitDepth:\\s*(\\d+)"));

        FRegexMatcher WidthMatcher(WidthPattern, MetadataContent);
        FRegexMatcher HeightMatcher(HeightPattern, MetadataContent);
        FRegexMatcher BitDepthMatcher(BitDepthPattern, MetadataContent);

        if (WidthMatcher.FindNext())
        {
            OutWidth = FCString::Atoi(*WidthMatcher.GetCaptureGroup(1));
        }
        if (HeightMatcher.FindNext())
        {
            OutHeight = FCString::Atoi(*HeightMatcher.GetCaptureGroup(1));
        }
        if (BitDepthMatcher.FindNext())
        {
            OutBitDepth = FCString::Atoi(*BitDepthMatcher.GetCaptureGroup(1));
        }

        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to read metadata file: %s"), *MetadataFilePath);
    return false;
}

bool UHeightmapParser::GuessDimensions(int64 FileSize, int32 BitDepth, int32& OutWidth, int32& OutHeight)
{
    int32 BytesPerPixel = BitDepth / 8;
    int32 NumSamples = FileSize / BytesPerPixel;

    TArray<int32> CommonWidths = {256, 512, 1024, 2048, 4096, 8192, 16384};
    for (int32 Width : CommonWidths)
    {
        int32 Height = NumSamples / Width;
        if (Width * Height == NumSamples)
        {
            OutWidth = Width;
            OutHeight = Height;
            return true; // Valid dimensions found
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Unable to guess dimensions for file size: %lld bytes, Bit depth: %d"), FileSize, BitDepth);
    return false;
}
