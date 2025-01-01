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
        : Albedo(0.0f),
          Altitude(0.0f),
          AnnualPrecipitation(FMath::Max(0.0f, 0.0f)),
          Aspect(0.0f),
          BiomeType("Ocean"),
          BiomeColor(FColor::Black),          
          CellType(ECellType::Land),
          ClosestOceanTemperature(0.0f),
          ClosestOceanCurrentType("Warm"),
          DistanceToOcean(FMath::Max(0.0f, FLT_MAX)),
          FlowDirection("Clockwise"),
          IsWindOnshore(false),
          Latitude(0.0f),
          Longitude(0.0f),          
          OceanDepth(FMath::Max(0.0f, 0.0f)),
          OceanToLandVector(FVector2D::ZeroVector),
          Slope(0.0f),
          Temperature(0.0f),
          WindDirection(FVector2D::ZeroVector)
          
    {}
    
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Albedo; // Reflectivity (0.0 - 1.0)
    
    /** Altitude or depth of the cell. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Altitude;    

    /**Annualized precipitation for the cell in millimeters*/
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float AnnualPrecipitation;

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Aspect; // Direction the slope faces (0-360°)

    /** Biome Type */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FString BiomeType;

    /** Biome Color */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FColor BiomeColor;

    /** Cell type, Land, Ocean, River or Lake */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    ECellType CellType;

     /** Ocean Current affecting land cell. Warm or Cold. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float ClosestOceanTemperature;

     /** Ocean Current affecting land cell. Warm or Cold. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float ClosestOceanCurrentType;

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

    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Slope; // Degrees of incline

    /**Temperature for a cell*/
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    float Temperature;

    /**Wind Direction. */
    UPROPERTY(BlueprintReadWrite, Category = "Heightmap")
    FVector2D WindDirection;
    
};
