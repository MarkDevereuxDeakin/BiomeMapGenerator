#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.generated.h"

/**
 * Enum representing a cell type in a heightmap.
 */
UENUM(BlueprintType)
enum class ECellType : uint8
{
    Land UMETA(DisplayName = "Land"),
    Ocean UMETA(DisplayName = "Ocean"),
    Lake UMETA(DisplayName = "Lake"),
    River UMETA(DisplayName = "River")
};

/**
 * Struct representing a cell in a heightmap.
 */
USTRUCT(BlueprintType) // Make the struct usable in Blueprints
struct BIOMEMAPPER_API FHeightmapCell
{
    public:

    /** Default constructor with initialization */
    FHeightmapCell()
        : Altitude(0.0f),
          CellType(ECellType::Ocean),
          DistanceToOcean(FMath::Max(0.0f, FLT_MAX)),
          FlowDirection("Clockwise"),
          IsWindOnshore(false),
          Latitude(0.0f),
          Longitude(0.0f),
          OceanDepth(FMath::Max(0.0f, 0.0f)),
          OceanToLandVector(FVector2D::ZeroVector),
          AnnualPrecipitation(FMath::Max(0.0f, 0.0f)),
          RelativeHumidity(FMath::Clamp(0.0f, 0.0f, 1.0f)),
          Temperature(0.0f),
          WindDirection(FVector2D::ZeroVector)
          
    {}
    
    GENERATED_BODY()

    /** Altitude or depth of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Altitude;

    /** Cell type, Land, Ocean, River or Lake */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    ECellType CellType;

    /** Distance to the nearest ocean pixel. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float DistanceToOcean;

    /** The ocean current flow direction for the nearest ocean pixel. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FString FlowDirection;

    /** Is the wind onshore (true) or offshore (false) based on OceanToLandVector and wind direction. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    bool IsWindOnshore;
    
    /** Geographic latitude of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Latitude;

    /** Geographic longitude of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Longitude;

    /** Depth of the ocean if the cell is underwater. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float OceanDepth;

    /** Define the ocean-to-land vector */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FVector2D OceanToLandVector;

    /**Annualized precipitation for the cell in millimeters*/
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float AnnualPrecipitation;

    /**Relative Humidity*/
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float RelativeHumidity;

    /**Temperature for a cell*/
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Temperature;

    /**Wind Direction. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FVector2D WindDirection;
    
};
