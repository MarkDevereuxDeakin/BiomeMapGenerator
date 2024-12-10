#include "LatitudeSliderWidget.h"
#include "MultiHandleSlider.h"

void SLatitudeSliderWidget::Construct(const FArguments& InArgs)
{
    MinLatitude = InArgs._InitialMinLatitude;
    MaxLatitude = InArgs._InitialMaxLatitude;
    OnLatitudeChanged = InArgs._OnLatitudeChanged;

    ChildSlot
    [
        SNew(SMultiHandleSlider)
        .MinValue(-90.0f)
        .MaxValue(90.0f)
        .HandleValues_Lambda([this]() -> TArray<float> {
            return {MinLatitude, MaxLatitude};
        })
        .OnValueChanged(this, &SLatitudeSliderWidget::HandleSliderValueChanged)
    ];
}

void SLatitudeSliderWidget::HandleSliderValueChanged(const TArray<float>& NewValues)
{
    if (NewValues.Num() >= 2)
    {
        MinLatitude = NewValues[0];
        MaxLatitude = NewValues[1];

        UE_LOG(LogTemp, Log, TEXT("Latitude Slider Updated: Min=%f, Max=%f"), MinLatitude, MaxLatitude);

        if (OnLatitudeChanged.IsBound())
        {
            OnLatitudeChanged.Execute();
        }
    }
}
