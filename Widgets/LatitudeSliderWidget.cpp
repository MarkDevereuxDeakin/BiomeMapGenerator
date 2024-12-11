#include "LatitudeSliderWidget.h"
#include "MultiHandleSlider.h"

void SLatitudeSliderWidget::Construct(const FArguments& InArgs)
{
    MinLatitude = InArgs._InitialMinLatitude;
    MaxLatitude = InArgs._InitialMaxLatitude;
    OnLatitudeChanged = InArgs._OnLatitudeChanged;

    ChildSlot
    [
        SNew(SHorizontalBox)

        // Vertical Text for Latitude
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)       
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .VAlign(VAlign_Center) // Center vertically
            .Padding(0.0f, 50.0f)         // Add vertical padding for centering
            [
                SNew(STextBlock)
                .Text(FText::FromString("Latitude"))
                .RenderTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(-90.0f)))) // Rotate 90 degrees
                .Justification(ETextJustify::Center)
                .Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 12)) // Adjust the size
            ]
        ]

        // The Slider
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SMultiHandleSlider)
            .MinValue(MinLatitude)
            .MaxValue(MaxLatitude)
            .HandleValues_Lambda([this]() -> TArray<float> {
                return {MinLatitude, MaxLatitude};
            })
            .OnValueChanged(this, &SLatitudeSliderWidget::HandleSliderValueChanged)
            .SliderInterval(10.0f)
            .LabelFormatter([](float Value) -> FString {
                return FString::Printf(TEXT("%.0f°"), Value); // Format as degrees
            })
        ]
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