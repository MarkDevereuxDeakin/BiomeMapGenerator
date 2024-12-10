#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Custom altitude slider widget with multiple handles.
 */
class SAltitudeSliderWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAltitudeSliderWidget) {}
        SLATE_ARGUMENT(float, InitialMinAltitude)
        SLATE_ARGUMENT(float, InitialMaxAltitude)
        SLATE_ARGUMENT(float, InitialSeaLevel)
        SLATE_EVENT(FSimpleDelegate, OnAltitudeChanged)
    SLATE_END_ARGS()

    /** Constructs the widget with the provided arguments. */
    void Construct(const FArguments& InArgs);

private:
    /** Handles value changes from the slider. */
    void HandleSliderValueChanged(const TArray<float>& NewValues);

    /** Minimum altitude value. */
    float MinAltitude;

    /** Maximum altitude value. */
    float MaxAltitude;

    /** Sea level value. */
    float SeaLevel;

    /** Delegate to trigger when altitude values are changed. */
    FSimpleDelegate OnAltitudeChanged;
};
