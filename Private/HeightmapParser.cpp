#include "HeightmapParser.h"
#include "Altitude.h"
#include "DistanceToOcean.h"

bool HeightmapParser::ParseHeightmap(
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
    // Load raw heightmap data
    TArray<uint8> HeightmapPixels;
    if (!HeightmapParser::LoadHeightmap(FilePath, HeightmapPixels, Width, Height))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load heightmap from file: %s"), *FilePath);
        return false;
    }

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
            Cell.Longitude = MinLongitude + (MaxLongitude - MinLongitude) * (x / static_cast<float>(Width));

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

bool HeightmapParser::LoadHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight)
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

bool HeightmapParser::ParseRawHeightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
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

bool HeightmapParser::ParseR16Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
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

bool HeightmapParser::ParseR32Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height)
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
