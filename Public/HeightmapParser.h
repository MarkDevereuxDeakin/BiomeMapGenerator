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
    static bool ParseRawHeightmap(TArray<uint8>& RawData, int32& Width, int32& Height);

   /**
     * Parses an R16 heightmap (16-bit).
     * @param RawData The raw byte data of the heightmap.
     * @param Width The width of the heightmap.
     * @param Height The height of the heightmap.
     * @return true if parsing was successful, false otherwise.
     */
    static bool ParseR16Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height);
   
    /**
     * Parses a raw 32-bit heightmap.
     * @param RawData The raw byte data of the heightmap.
     * @param Width The width of the heightmap.
     * @param Height The height of the heightmap.
     * @return true if parsing was successful, false otherwise.
     */
    static bool ParseR32Heightmap(TArray<uint8>& RawData, int32& Width, int32& Height);


    static bool ParseImageHeightmap(const FString& FilePath, TArray<uint8>& OutHeightmapData, int32& OutWidth, int32& OutHeight);

    static bool IsBigEndian(const TArray<uint8>& RawData, int32 BitDepth);

    static void ConvertToLittleEndian(TArray<uint8>& RawData);

    /**
     * Converts 32-bit raw data from big-endian to little-endian format.
     * @param RawData The raw byte data of the heightmap.
     */
    static void ConvertToLittleEndian32(TArray<uint8>& RawData);

};
