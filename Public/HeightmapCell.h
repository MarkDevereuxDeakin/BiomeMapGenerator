#pragma once
#include "CoreMinimal.h"
#include "HeightmapCell.generated.h"

USTRUCT(BlueprintType)// Make the struct usable in Blueprints
struct BIOMEMAPPER_API FHeightmapCell
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Latitude;       // Geographic latitude of the cell

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Longitude;      // Geographic longitude of the cell

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Altitude;       // Altitude or depth of the cell

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float DistanceToOcean; // Distance to the nearest ocean pixel

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float OceanDepth;     // Depth of the ocean if underwater
};
