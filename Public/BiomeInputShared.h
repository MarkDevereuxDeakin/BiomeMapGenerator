#pragma once

#include "CoreMinimal.h"
#include "BiomeInputShared.generated.h"

// Centralized structure for user inputs
USTRUCT(BlueprintType) // Make the struct usable in Blueprints
struct BIOMEMAPPER_API FInputParameters
{
  public:

    FInputParameters()
      : CentralLongitude(0.0f),
        NorthernLatitude(0.0f),
        SouthernLatitude(0.0f),
        MaximumAltitude(0.0f),
        MinimumAltitude(0.0f),
        SeaLevel(0.0f)
        
    {}

    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float CentralLongitude;

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float NorthernLatitude;

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float SouthernLatitude;

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float MaximumAltitude;

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float MinimumAltitude;

    UPROPERTY(BlueprintReadWrite, Category = "Input Parameters")
    float SeaLevel;  
};


