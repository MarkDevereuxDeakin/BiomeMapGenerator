#include "AltitudeSliderWidget.h"
#include "MultiHandleSlider.h"

void SAltitudeSliderWidget::Construct(const FArguments& InArgs)
{
    MinAltitude = InArgs._InitialMinAltitude;
    MaxAltitude = InArgs._InitialMaxAltitude;
    SeaLevel = InArgs._InitialSeaLevel;
    OnAltitudeChanged = InArgs._OnAltitudeChanged;

    ChildSlot
    [
        SNew(SMultiHandleSlider)
        .MinValue(MinAltitude)
        .MaxValue(MaxAltitude)
        .HandleValues_Lambda([this]() -> TArray<float> {
            return {MinAltitude, MaxAltitude, SeaLevel};
        })
        .OnValueChanged(this, &SAltitudeSliderWidget::HandleSliderValueChanged)
        .SliderInterval(200.0f)
        .LabelFormatter([](float Value) -> FString {
            return FString::Printf(TEXT("%.0fm"), Value); // Format as meters
        })
    ];
}


void SAltitudeSliderWidget::HandleSliderValueChanged(const TArray<float>& NewValues)
{
    if (NewValues.Num() >= 3)
    {
        // Update the slider values
        MinAltitude = NewValues[0];
        MaxAltitude = NewValues[1];
        SeaLevel = NewValues[2];

        // Log updates
        UE_LOG(LogTemp, Log, TEXT("Altitude Slider Updated: Min=%f, Max=%f, Sea Level=%f"), MinAltitude, MaxAltitude, SeaLevel);

        // Execute bound delegate, if any
        if (OnAltitudeChanged.IsBound())
        {
            OnAltitudeChanged.Execute();
        }
    }
}