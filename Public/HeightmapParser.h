#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"

/**
 * Utility class for parsing heightmaps into usable data.
 */
class BIOMEMAPPER_API HeightmapParser
{
public:
    /**
     * Main function to parse a heightmap file.
     * @param FilePath - Path to the heightmap file.
     * @param SeaLevel - Defined sea level for the heightmap.
     * @param MinAltitude - Minimum altitude in the heightmap.
     * @param MaxAltitude - Maximum altitude in the heightmap.
     * @param MinLatitude - Minimum latitude of the region.
     * @param MaxLatitude - Maximum latitude of the region.
     * @param OutMinLongitude - Output minimum longitude estimate.
     * @param OutMaxLongitude - Output maximum longitude estimate.
     * @param OutData - Output array of parsed heightmap cells.
     * @param Width - Width of the heightmap (in pixels).
     * @param Height - Height of the heightmap (in pixels).
     * @return True if parsing was successful.
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
        int32& Height);

private:
    /**
     * Estimate the longitude range for a heightmap.
     * @param MinLatitude - Minimum latitude.
     * @param MaxLatitude - Maximum latitude.
     * @param Width - Width of the heightmap (pixels).
     * @param Height - Height of the heightmap (pixels).
     * @param OutMinLongitude - Output minimum longitude estimate.
     * @param OutMaxLongitude - Output maximum longitude estimate.
     */

    static void EstimateLongitudeRange(
        float MinLatitude,
        float MaxLatitude,
        int32 Width,
        int32 Height,
        float& OutMinLongitude,
        float& OutMaxLongitude);

    static bool LoadHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight);

    /** Parsing methods for specific formats */
    static bool ParseRawHeightmap(TArray<uint8>& RawData, int32& Width, int32& Height);
    static bool ParseR16Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height);
    static bool ParseR32Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height);
    static bool ParseImageHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight);

    /** Determines if the data is in big-endian format. */
    static bool IsBigEndian(const TArray<uint8>& RawData, int32 BitDepth);

    /** Converts raw data to little-endian format. */
    static void ConvertToLittleEndian(TArray<uint8>& RawData);
    static void ConvertToLittleEndian32(TArray<uint8>& RawData);
};
