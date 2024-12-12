#include "HeightmapParser.h"
#include "Altitude.h"
#include "DistanceToOcean.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

bool UHeightmapParser::ParseHeightmap(
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
    TArray<float> RawData;
    int32 BitDepth = 0;

    if (!LoadHeightmap(FilePath, RawData, Width, Height, BitDepth))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap: %s"), *FilePath);
        return false;
    }

    if (Width <= 0 || Height <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid heightmap dimensions: %dx%d"), Width, Height);
        return false;
    }

    EstimateLongitudeRange(MinLatitude, MaxLatitude, Width, Height, OutMinLongitude, OutMaxLongitude);

    OutData.Empty(Width * Height);
    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;
            uint8 PixelValue = RawData[Index];

            FHeightmapCell Cell;
            Cell.Latitude = MinLatitude + (MaxLatitude - MinLatitude) * (y / static_cast<float>(Height));
            Cell.Longitude = OutMinLongitude + (OutMaxLongitude - OutMinLongitude) * (x / static_cast<float>(Width));
            Cell.Altitude = CalculateAltitude(PixelValue, MinAltitude, MaxAltitude);

            if (Cell.Altitude <= SeaLevel)
            {
                Cell.OceanDepth = CalculateOceanDepth(SeaLevel, Cell.Altitude);
                Cell.DistanceToOcean = 0.0f;
            }
            else
            {
                Cell.OceanDepth = 0.0f;
                Cell.DistanceToOcean = FLT_MAX;
            }

            OutData.Add(Cell);
        }
    }

    if (!CalculateDistanceToOcean(OutData, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to calculate distances to ocean."));
        return false;
    }

    return true;
}

bool UHeightmapParser::LoadHeightmap(
    const FString& FilePath,
    TArray<float>& OutHeightmapData,
    int32& OutWidth,
    int32& OutHeight,
    int32& BitDepth)
{
    FString FileExtension = FPaths::GetExtension(FilePath).ToLower();

    if (FileExtension == TEXT("png") || FileExtension == TEXT("jpg") || FileExtension == TEXT("jpeg"))
    {
        return ParseImageHeightmap(FilePath, OutHeightmapData, OutWidth, OutHeight, BitDepth);
    }
    else if (FileExtension == TEXT("r16") || FileExtension == TEXT("r32") || FileExtension == TEXT("raw"))
    {
        BitDepth = (FileExtension == TEXT("r16")) ? 16 : 32;
        return ParseRawHeightmap(FilePath, OutHeightmapData, OutWidth, OutHeight, BitDepth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported file format: %s"), *FileExtension);
        return false;
    }
}

bool UHeightmapParser::ParseImageHeightmap(
    const FString& FilePath,
    TArray<float>& OutHeightmapData,
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

    if (BitDepth == 16)
    {
        TArray<uint8> RawByteData;
        if (!ImageWrapper->GetRaw(ERGBFormat::Gray, 16, RawByteData))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to decompress 16-bit image: %s"), *FilePath);
            return false;
        }

        // Convert raw byte data directly to normalized floating-point values
        int32 NumPixels = RawByteData.Num() / 2; // Each pixel is 2 bytes
        OutHeightmapData.Reserve(NumPixels);

        for (int32 i = 0; i < RawByteData.Num(); i += 2)
        {
            uint16 Value = (RawByteData[i + 1] << 8) | RawByteData[i]; // Combine bytes into a 16-bit integer
            OutHeightmapData.Add(static_cast<float>(Value) / 65535.0f); // Normalize to [0.0, 1.0] and store as float
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

        OutHeightmapData.Reserve(Raw8BitData.Num());
        for (uint8 Value : Raw8BitData)
        {
            OutHeightmapData.Add(Value / 255.0f); // Normalize to [0.0, 1.0]
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
    int32& Width,
    int32& Height,
    int32 BitDepth)
{
    TArray<uint8> RawData;
    if (!FFileHelper::LoadFileToArray(RawData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load raw heightmap file: %s"), *FilePath);
        return false;
    }

    int32 BytesPerPixel = BitDepth / 8;
    int32 NumSamples = RawData.Num() / BytesPerPixel;

    Width = FMath::Sqrt(static_cast<float>(NumSamples));
    Height = NumSamples / Width;

    if (Width * Height != NumSamples)
    {
        UE_LOG(LogTemp, Error, TEXT("Raw heightmap data is not a perfect rectangle."));
        return false;
    }

    if (BitDepth == 16)
    {
        OutHeightmapData.Reserve(NumSamples);
        for (int32 i = 0; i < RawData.Num(); i += 2)
        {
            uint16 Value = (RawData[i + 1] << 8) | RawData[i];
            OutHeightmapData.Add(Value / 65535.0f); // Normalize 16-bit to [0.0, 1.0]
        }
    }
    else if (BitDepth == 32)
    {
        OutHeightmapData.Reserve(NumSamples);
        for (int32 i = 0; i < RawData.Num(); i += 4)
        {
            uint32 Value = (RawData[i + 3] << 24) | (RawData[i + 2] << 16) | (RawData[i + 1] << 8) | RawData[i];
            OutHeightmapData.Add(*reinterpret_cast<float*>(&Value)); // Store as-is
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported bit depth: %d"), BitDepth);
        return false;
    }

    return true;
}


void UHeightmapParser::EstimateLongitudeRange(
    float MinLatitude,
    float MaxLatitude,
    int32 Width,
    int32 Height,
    float& OutMinLongitude,
    float& OutMaxLongitude)
{
    float AspectRatio = Width / static_cast<float>(Height);
    float AverageLatitude = (MinLatitude + MaxLatitude) / 2.0f;
    OutMinLongitude = -AspectRatio * 180.0f / FMath::Cos(FMath::DegreesToRadians(AverageLatitude));
    OutMaxLongitude = AspectRatio * 180.0f / FMath::Cos(FMath::DegreesToRadians(AverageLatitude));
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