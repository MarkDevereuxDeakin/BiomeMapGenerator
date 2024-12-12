// HeightmapParser.h
#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h" // Assume this file defines FHeightmapCell
#include "HeightmapParser.generated.h"

/**
 * HeightmapParser handles parsing of various heightmap formats
 * including PNG, JPG, R16, R32, and raw binary heightmaps.
 */
UCLASS()
class BIOMEMAPPER_API UHeightmapParser : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Parses a heightmap file into structured data.
     * @param FilePath - Path to the heightmap file.
     * @param SeaLevel - Altitude defining sea level.
     * @param MinAltitude - Minimum altitude for normalization.
     * @param MaxAltitude - Maximum altitude for normalization.
     * @param MinLatitude - Minimum latitude for the heightmap.
     * @param MaxLatitude - Maximum latitude for the heightmap.
     * @param OutMinLongitude - Calculated minimum longitude.
     * @param OutMaxLongitude - Calculated maximum longitude.
     * @param OutData - Parsed heightmap cell data.
     * @param Width - Output width of the heightmap.
     * @param Height - Output height of the heightmap.
     * @return True if parsing is successful.
     */
    static bool ParseHeightmap(
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
        int32& Height
    );

private:
    // Helper functions

    /**
     * Loads heightmap data from a file.
     * @return True if loading is successful.
     */
    static bool LoadHeightmap(const FString& FilePath,
    TArray<float>& OutHeightmapData,
    int32& OutWidth,
    int32& OutHeight,
    int32& BitDepth);

    /**
     * Parses image-based heightmaps (PNG, JPG).
     */
    static bool ParseImageHeightmap(const FString& FilePath,
    TArray<float>& OutHeightmapData,
    int32& OutWidth,
    int32& OutHeight,
    int32& BitDepth);

    /**
     * Parses raw binary heightmaps (R16, R32).
     */
    static bool ParseRawHeightmap(const FString& FilePath,
    TArray<float>& OutHeightmapData,
    int32& Width,
    int32& Height,
    int32 BitDepth);

    /**
     * Calculates longitude range for the heightmap.
     */
    static void EstimateLongitudeRange(
        float MinLatitude,
        float MaxLatitude,
        int32 Width,
        int32 Height,
        float& OutMinLongitude,
        float& OutMaxLongitude
    );
    
    /**
     * Checks if the raw data is big-endian.
     */
    static bool IsBigEndian(const TArray<uint8>& Data, int32 BitDepth);

    /**
     * Converts 16-bit raw data from big-endian to little-endian.
     */
    static void ConvertToLittleEndian(TArray<uint8>& Data);

    /**
     * Converts 32-bit raw data from big-endian to little-endian.
     */
    static void ConvertToLittleEndian32(TArray<uint8>& Data);
};