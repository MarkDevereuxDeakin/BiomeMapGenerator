#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.generated.h"

/**
 * Struct representing a cell in a heightmap.
 */
USTRUCT(BlueprintType) // Make the struct usable in Blueprints
struct BIOMEMAPPER_API FHeightmapCell
{
    public:

    /** Default constructor with initialization */
    FHeightmapCell()
        : Latitude(0.0f),
          Longitude(0.0f),
          Altitude(0.0f),
          DistanceToOcean(FLT_MAX),
          OceanDepth(0.0f)
    {}
    
    GENERATED_BODY()

    /** Geographic latitude of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Latitude;

    /** Geographic longitude of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Longitude;

    /** Altitude or depth of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Altitude;

    /** Distance to the nearest ocean pixel. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float DistanceToOcean;

    /** Depth of the ocean if the cell is underwater. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float OceanDepth;
};
