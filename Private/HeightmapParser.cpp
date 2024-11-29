#include "HeightmapParser.h"
#include "Altitude.h"
#include "DistanceToOcean.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

bool HeightmapParser::ParseHeightmap(
   const FString& FilePath,
    float SeaLevel,
    float MinAltitude,
    float MaxAltitude,
    float MinLatitude,
    float MaxLatitude,
    float& OutMinLongitude,
    float& OutMaxLongitude,
    TArray<FHeightmapCell>& OutData,
    int32& Width,
    int32& Height)
{
    // Load raw heightmap data
    TArray<uint8> HeightmapPixels;
    if (!HeightmapParser::LoadHeightmap(FilePath, HeightmapPixels, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap from file: %s"), *FilePath);
        return false;
    }

    // Estimate longitude range
    EstimateLongitudeRange(MinLatitude, MaxLatitude, Width, Height, OutMinLongitude, OutMaxLongitude);

    // Further processing of the heightmap data...
    // Populate OutData with FHeightmapCell values based on loaded data.

    // Prepare output array

    OutData.Empty(Width * Height);

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;
            uint8 PixelValue = HeightmapPixels[Index];

            FHeightmapCell Cell;

            // Compute latitude and longitude
            Cell.Latitude = MinLatitude + (MaxLatitude - MinLatitude) * (y / static_cast<float>(Height));
            Cell.Longitude = OutMinLongitude + (OutMaxLongitude - OutMinLongitude) * (x / static_cast<float>(Width));

            // Calculate altitude
            Cell.Altitude = CalculateAltitude(PixelValue, MinAltitude, MaxAltitude);

            // Determine if land or ocean
            if (Cell.Altitude <= SeaLevel)
            {
                Cell.OceanDepth = CalculateOceanDepth(SeaLevel, Cell.Altitude);
                Cell.DistanceToOcean = 0.0f;
            }
            else
            {
                Cell.OceanDepth = 0.0f;
                Cell.DistanceToOcean = FLT_MAX; // Mark land for distance calculation
            }

            OutData.Add(Cell);
        }
    }

    // Calculate distances to ocean for all land pixels
    if (!CalculateDistanceToOcean(OutData, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to calculate distance to ocean."));
        return false;
    }

    return true;
}

void HeightmapParser::EstimateLongitudeRange(
    float MinLatitude,
    float MaxLatitude,
    int32 Width,
    int32 Height,
    float& OutMinLongitude,
    float& OutMaxLongitude)
{
    // Calculate latitude per pixel
    float LatitudeRange = MaxLatitude - MinLatitude;
    float LatitudePerPixel = LatitudeRange / Height;

    // Calculate the average latitude for the heightmap
    float AverageLatitude = (MinLatitude + MaxLatitude) / 2.0f;

    // Convert average latitude to radians
    float AverageLatitudeRadians = FMath::DegreesToRadians(AverageLatitude);

    // Calculate longitude per pixel at the average latitude
    float LongitudePerPixel = LatitudePerPixel / FMath::Cos(AverageLatitudeRadians);

    // Calculate total longitude range
    float LongitudeRange = LongitudePerPixel * Width;

    // Estimate min and max longitude
    OutMinLongitude = -LongitudeRange / 2.0f; // Centering longitude at 0
    OutMaxLongitude = LongitudeRange / 2.0f;
}

bool HeightmapParser::LoadHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight)
{
    // Get file extension
    FString FileExtension = FPaths::GetExtension(FilePath).ToLower();

    // Load the file into memory
    if (!FFileHelper::LoadFileToArray(OutHeightmapData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap file: %s"), *FilePath);
        return false;
    }

    // Handle supported formats
    if (FileExtension == TEXT("png") || FileExtension == TEXT("jpg") || FileExtension == TEXT("jpeg"))
    {
        return ParseImageHeightmap(FilePath, OutHeightmapData, OutWidth, OutHeight);
    }
    else if (FileExtension == TEXT("r16"))
    {
        return ParseR16Heightmap(OutHeightmapData, OutWidth, OutHeight);
    }
    else if (FileExtension == TEXT("r32"))
    {
        return ParseR32Heightmap(OutHeightmapData, OutWidth, OutHeight);
    }
    else if (FileExtension == TEXT("raw"))
    {
        // Detect bit depth and parse accordingly
        int32 DataSize = OutHeightmapData.Num();
        int32 NumSamples16 = FMath::Sqrt(static_cast<float>(DataSize / 2)); // 16-bit assumption
        int32 NumSamples32 = FMath::Sqrt(static_cast<float>(DataSize / 4)); // 32-bit assumption

        if (NumSamples16 * NumSamples16 * 2 == DataSize)
        {
            return ParseR16Heightmap(OutHeightmapData, OutWidth, OutHeight);
        }
        else if (NumSamples32 * NumSamples32 * 4 == DataSize)
        {
            return ParseR32Heightmap(OutHeightmapData, OutWidth, OutHeight);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unsupported raw file size: %d bytes. Must be 16-bit or 32-bit square."), DataSize);
            return false;
        }
    }

    // Unsupported file extension
    UE_LOG(LogTemp, Error, TEXT("Unsupported file extension: %s"), *FileExtension);
    return false;
}


bool HeightmapParser::ParseRawHeightmap(TArray<uint8>& RawData, int32& Width, int32& Height)
{
    if (RawData.Num() % 2 == 0) // Assume 16-bit if divisible by 2
    {
        int32 NumSamples = RawData.Num() / 2;

        Width = FMath::Sqrt(static_cast<float>(NumSamples));
        Height = Width;

        if (Width * Height != NumSamples)
        {
            UE_LOG(LogTemp, Error, TEXT("ParseRawHeightmap: Data does not represent a square heightmap."));
            return false;
        }

        if (IsBigEndian(RawData, 16))
        {
            UE_LOG(LogTemp, Log, TEXT("Raw data is 16-bit big-endian. Conversion required."));
            ConvertToLittleEndian(RawData);
        }

        return true;
    }
    else if (RawData.Num() % 4 == 0) // Assume 32-bit if divisible by 4
    {
        int32 NumSamples = RawData.Num() / 4;

        Width = FMath::Sqrt(static_cast<float>(NumSamples));
        Height = Width;

        if (Width * Height != NumSamples)
        {
            UE_LOG(LogTemp, Error, TEXT("ParseRawHeightmap: Data does not represent a square heightmap."));
            return false;
        }

        if (IsBigEndian(RawData, 32))
        {
            UE_LOG(LogTemp, Log, TEXT("Raw data is 32-bit big-endian. Conversion required."));
            ConvertToLittleEndian32(RawData);
        }

        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("ParseRawHeightmap: Unsupported raw file size. Must be 16-bit or 32-bit."));
    return false;
}


bool HeightmapParser::ParseR16Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height)
{
    if (RawData.Num() % 2 != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR16Heightmap: Data size is not a multiple of 2 bytes."));
        return false;
    }

    int32 NumSamples = RawData.Num() / 2;

    // Ensure the data represents a square heightmap
    Width = FMath::Sqrt(static_cast<float>(NumSamples));
    Height = Width;

    if (Width * Height != NumSamples)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR16Heightmap: Data does not represent a square heightmap."));
        return false;
    }

    // Detect and convert endianness
    if (IsBigEndian(RawData, 16))
    {
        UE_LOG(LogTemp, Log, TEXT("Data is big-endian. Converting to little-endian."));
        ConvertToLittleEndian(RawData);
    }

    return true;
}

bool HeightmapParser::ParseR32Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height)
{
    if (RawData.Num() % sizeof(float) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR32Heightmap: Data size is not a multiple of 4 bytes."));
        return false;
    }

    int32 NumSamples = RawData.Num() / sizeof(float);

    // Ensure the data represents a square heightmap
    Width = FMath::Sqrt(static_cast<float>(NumSamples));
    Height = Width;

    if (Width * Height != NumSamples)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR32Heightmap: Data does not represent a square heightmap."));
        return false;
    }

    // Detect and convert endianness
    if (IsBigEndian(RawData, 32))
    {
        UE_LOG(LogTemp, Log, TEXT("Data is big-endian. Converting to little-endian."));
        ConvertToLittleEndian32(RawData);
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully parsed 32-bit raw heightmap data. Dimensions: %dx%d"), Width, Height);
    return true;
}

bool HeightmapParser::ParseImageHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight)
{
    // Load file into binary array
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load image file: %s"), *FilePath);
        return false;
    }

    // Get the image wrapper module
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

    // Extract dimensions
    OutWidth = ImageWrapper->GetWidth();
    OutHeight = ImageWrapper->GetHeight();

    ERGBFormat DetectedFormat = ImageWrapper->GetFormat();
    int32 BitDepth = ImageWrapper->GetBitDepth();

    if (DetectedFormat != ERGBFormat::Gray || (BitDepth != 8 && BitDepth != 16))
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported image format or bit depth: Format=%d, BitDepth=%d"), static_cast<int32>(DetectedFormat), BitDepth);
        return false;
    }

    // Decompress the image
    TArray<uint8> RawData;
    if (!ImageWrapper->GetRaw(ERGBFormat::Gray, BitDepth, RawData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decompress image: %s"), *FilePath);
        return false;
    }

    // Process 16-bit data
    if (BitDepth == 16)
    {
        int32 NumPixels = RawData.Num() / 2; // Each pixel is 2 bytes
        OutHeightmapData.Reserve(NumPixels);

        for (int32 i = 0; i < RawData.Num(); i += 2)
        {
            uint16 PixelValue = (RawData[i + 1] << 8) | RawData[i]; // Convert to 16-bit
            OutHeightmapData.Add(static_cast<uint8>(PixelValue >> 8)); // Scale down to 8-bit
        }
    }
    else
    {
        // Directly use 8-bit data
        OutHeightmapData = MoveTemp(RawData);
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully parsed image file: %s"), *FilePath);
    return true;
}

bool HeightmapParser::IsBigEndian(const TArray<uint8>& RawData, int32 BitDepth)
{
    // Ensure the raw data is large enough to perform checks
    int32 MinBytes = (BitDepth == 16) ? 2 : 4;
    if (RawData.Num() < MinBytes)
    {
        UE_LOG(LogTemp, Error, TEXT("Raw data is too small to determine endianness."));
        return false;
    }

    if (BitDepth == 16)
    {
        uint16 FirstSample = (RawData[0] << 8) | RawData[1]; // Big-endian representation
        uint16 TestValue = *reinterpret_cast<const uint16*>(RawData.GetData()); // Native representation

        return FirstSample != TestValue; // True if data is big-endian
    }
    else if (BitDepth == 32)
    {
        uint32 FirstSample = (RawData[0] << 24) | (RawData[1] << 16) | (RawData[2] << 8) | RawData[3]; // Big-endian representation
        uint32 TestValue = *reinterpret_cast<const uint32*>(RawData.GetData()); // Native representation

        return FirstSample != TestValue; // True if data is big-endian
    }

    // Unsupported bit depth
    UE_LOG(LogTemp, Error, TEXT("Unsupported bit depth: %d"), BitDepth);
    return false;
}


void HeightmapParser::ConvertToLittleEndian(TArray<uint8>& RawData)
{
    if (RawData.Num() % 2 != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Raw data size is not a multiple of 2. Cannot process as 16-bit data."));
        return;
    }

    int32 NumSamples = RawData.Num() / 2;

    ParallelFor(NumSamples, [&](int32 i)
    {
        int32 ByteIndex = i * 2;
        uint8 HighByte = RawData[ByteIndex];
        uint8 LowByte = RawData[ByteIndex + 1];

        // Swap bytes to convert to little-endian
        RawData[ByteIndex] = LowByte;
        RawData[ByteIndex + 1] = HighByte;
    });

    UE_LOG(LogTemp, Log, TEXT("Converted 16-bit data to little-endian format."));
}


void HeightmapParser::ConvertToLittleEndian32(TArray<uint8>& RawData)
{
    if (RawData.Num() % 4 != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Raw data size is not a multiple of 4. Cannot process as 32-bit data."));
        return;
    }

    int32 NumSamples = RawData.Num() / 4;

    ParallelFor(NumSamples, [&](int32 i)
    {
        int32 ByteIndex = i * 4;
        uint8 Byte0 = RawData[ByteIndex];
        uint8 Byte1 = RawData[ByteIndex + 1];
        uint8 Byte2 = RawData[ByteIndex + 2];
        uint8 Byte3 = RawData[ByteIndex + 3];

        // Swap bytes to convert to little-endian
        RawData[ByteIndex] = Byte3;
        RawData[ByteIndex + 1] = Byte2;
        RawData[ByteIndex + 2] = Byte1;
        RawData[ByteIndex + 3] = Byte0;
    });

    UE_LOG(LogTemp, Log, TEXT("Converted 32-bit data to little-endian format."));
}


