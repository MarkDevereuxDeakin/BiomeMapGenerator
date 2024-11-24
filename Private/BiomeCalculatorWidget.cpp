#include "BiomeCalculatorWidget.h"
#include "Widgets/Text/STextBlock.h"

void BiomeCalculatorWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(STextBlock).Text(FText::FromString("Biome Calculator Widget"))
    ];
}

void BiomeCalculatorWidget::SetHeightmapData(const TArray<FHeightmapCell>& InHeightmapData)
{
    HeightmapData = InHeightmapData;
}


/*#include "BiomeCalculatorWidget.h"
#include "BiomeCalculator.h"
#include "HeightmapParser.h"
#include "HeightmapCell.h"
#include "Misc/FileHelper.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Misc/Paths.h"

void UBiomeCalculatorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    BiomeCalculator = NewObject<UBiomeCalculator>();

    if (UploadButton)
    {
        UploadButton->OnClicked.AddDynamic(this, &UBiomeCalculatorWidget::OnUploadButtonClicked);
    }

    if (CalculateButton)
    {
        CalculateButton->OnClicked.AddDynamic(this, &UBiomeCalculatorWidget::OnCalculateBiomeClicked);
    }
}

void UBiomeCalculatorWidget::OnUploadButtonClicked()
{
    FString FilePath; // File selection logic here
    int32 Width, Height;

    if (ParseHeightmap(FilePath, HeightmapData, Width, Height))
    {
        UploadStatusText->SetText(FText::FromString("Heightmap loaded successfully."));
    }
    else
    {
        UploadStatusText->SetText(FText::FromString("Failed to parse heightmap."));
    }
}

void UBiomeCalculatorWidget::OnCalculateBiomeClicked()
{
    if (!BiomeCalculator || HeightmapData.Num() == 0)
    {
        ResultText->SetText(FText::FromString("No data available for biome calculation."));
        return;
    }

    FString Result = BiomeCalculator->CalculateBiomeFromInput(
        MinLatitudeInput->GetText().ToString(),
        MaxLatitudeInput->GetText().ToString(),
        MinLongitudeInput->GetText().ToString(),
        MaxLongitudeInput->GetText().ToString(),
        MinAltitudeInput->GetText().ToString(),
        MaxAltitudeInput->GetText().ToString(),
        SeaLevelInput->GetText().ToString(),
        HeightmapData);

    ResultText->SetText(FText::FromString(Result));
}*/