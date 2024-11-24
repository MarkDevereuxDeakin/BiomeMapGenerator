#include "HeightmapParser.h"
#include "Altitude.h"
#include "DistanceToOcean.h"

bool ParseHeightmap(
    const FString& FilePath,
    float SeaLevel,
    float MinAltitude,
    float MaxAltitude,
    float MinLatitude,
    float MaxLatitude,
    float MinLongitude,
    float MaxLongitude,
    TArray<FHeightmapCell>& OutData,
    int32& Width,
    int32& Height)
{
    // Load heightmap pixels
    TArray<uint8> HeightmapPixels;
    if (!LoadHeightmap(FilePath, HeightmapPixels, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap from file: %s"), *FilePath);
        return false;
    }

    // Prepare output data
    OutData.Empty(Width * Height);

    // Process each pixel
    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;
            uint8 PixelValue = HeightmapPixels[Index];

            FHeightmapCell Cell;

            // Calculate latitude and longitude
            Cell.Latitude = MinLatitude + (MaxLatitude - MinLatitude) * (y / (float)Height);
            Cell.Longitude = MinLongitude + (MaxLongitude - MinLongitude) * (x / (float)Width);

            // Calculate altitude
            Cell.Altitude = CalculateAltitude(PixelValue, MinAltitude, MaxAltitude);

            // Determine if land or ocean
            if (PixelValue >= SeaLevel)
            {
                Cell.OceanDepth = 0.0f;         // No depth for land
                Cell.DistanceToOcean = FLT_MAX; // Mark for distance calculation
            }
            else
            {
                Cell.OceanDepth = CalculateOceanDepth(SeaLevel, Cell.Altitude);
                Cell.DistanceToOcean = 0.0f;   // Ocean has no distance to itself
            }

            OutData.Add(Cell);
        }
    }

    // Calculate distance to ocean for land pixels
    if (!CalculateDistanceToOcean(OutData, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to calculate DistanceToOcean."));
        return false;
    }

    return true;
}

bool LoadHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight)
{
    // Read file into raw data
    if (!FFileHelper::LoadFileToArray(OutHeightmapData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap file: %s"), *FilePath);
        return false;
    }

    // Assume square heightmap for simplicity
    int32 NumPixels = OutHeightmapData.Num();
    OutWidth = FMath::Sqrt(static_cast<float>(NumPixels));
    OutHeight = OutWidth;

    if (OutWidth * OutHeight != NumPixels)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid heightmap dimensions in file: %s"), *FilePath);
        return false;
    }

    return true;
}

bool ParseRawHeightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
{
    int32 NumPixels = RawData.Num();

    // Ensure the data represents a square heightmap
    Width = FMath::Sqrt(static_cast<float>(NumPixels));
    Height = Width;

    if (Width * Height != NumPixels)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseRawHeightmap: Data does not represent a square heightmap."));
        return false;
    }

    return true;
}

bool ParseR16Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
{
    // Ensure the data is a multiple of 2 bytes (16 bits per sample)
    if (RawData.Num() % sizeof(uint16) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR16Heightmap: Data size is not a multiple of 2 bytes."));
        return false;
    }

    int32 NumSamples = RawData.Num() / sizeof(uint16);

    // Ensure the data represents a square heightmap
    Width = FMath::Sqrt(static_cast<float>(NumSamples));
    Height = Width;

    if (Width * Height != NumSamples)
    {
        UE_LOG(LogTemp, Error, TEXT("ParseR16Heightmap: Data does not represent a square heightmap."));
        return false;
    }

    return true;
}

bool ParseR32Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
{
    // Ensure the data is a multiple of 4 bytes (32 bits per sample)
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

    return true;
}
