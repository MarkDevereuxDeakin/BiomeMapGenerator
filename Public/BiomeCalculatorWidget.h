#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"
#include "Widgets/SCompoundWidget.h"

class BIOMEMAPPER_API BiomeCalculatorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(BiomeCalculatorWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetHeightmapData(const TArray<FHeightmapCell>& InHeightmapData);

private:
    TArray<FHeightmapCell> HeightmapData;
};


/*#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "HeightmapCell.h"
#include "BiomeCalculatorWidget.generated.h"

// Forward declarations
class UEditableTextBox;
class UTextBlock;
class UVerticalBox;
class UButton;
class UBiomeCalculator;

UCLASS()
class BIOMEMAPPER_API UBiomeCalculatorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
       
    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    void OnCalculateBiomeClicked();

    UFUNCTION(BlueprintCallable, Category = "Biome Calculator")
    void OnUploadButtonClicked();


protected:
    // Override to set up widget components
    virtual void NativeConstruct() override;

private:

    // Input fields
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MinLatitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MaxLatitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MinLongitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MaxLongitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MinAltitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MaxAltitudeInput;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* SeaLevelInput;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UploadStatusText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultText;

    UPROPERTY(meta = (BindWidget))
    UButton* UploadButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CalculateButton;

    // Biome calculator
    UPROPERTY()
    UBiomeCalculator* BiomeCalculator;

    // Parsed heightmap data
    TArray<FHeightmapCell> HeightmapData;   

};*/