#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"

/**
 * HeightmapParser: A utility class for parsing heightmaps.
 */
class BIOMEMAPPER_API HeightmapParser
{
    public:
    static bool ParseHeightmap(
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
        int32& Height);

    private:

    static bool LoadHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight);

        /**
     * Parses a raw heightmap (8-bit).
     * @param RawData The raw byte data of the heightmap.
     * @param Width The width of the heightmap.
     * @param Height The height of the heightmap.
     * @return true if parsing was successful, false otherwise.
     */
    static bool ParseRawHeightmap(const TArray<uint8>& RawData, int32& Width, int32& Height);

    /**
     * Parses an r16 heightmap (16-bit).
     * @param RawData The raw byte data of the heightmap.
     * @param Width The width of the heightmap.
     * @param Height The height of the heightmap.
     * @return true if parsing was successful, false otherwise.
     */
    static bool ParseR16Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height);

    /**
     * Parses an r32 heightmap (32-bit float).
     * @param RawData The raw byte data of the heightmap.
     * @param Width The width of the heightmap.
     * @param Height The height of the heightmap.
     * @return true if parsing was successful, false otherwise.
     */
    static bool ParseR32Heightmap(const TArray<uint8>& RawData, int32& Width, int32& Height);
};
