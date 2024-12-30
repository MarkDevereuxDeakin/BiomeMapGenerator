#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * A widget for the Latitude Slider with Min and Max handles.
 */
class SLatitudeSliderWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SLatitudeSliderWidget) {}
        SLATE_ARGUMENT(float, InitialMinLatitude)
        SLATE_ARGUMENT(float, InitialMaxLatitude)
        SLATE_ARGUMENT(UWorld*, WorldContext);
        SLATE_EVENT(FSimpleDelegate, OnLatitudeChanged)
    SLATE_END_ARGS()

    /** Constructs the Latitude Slider widget. */
    void Construct(const FArguments& InArgs);

    float GetMinLatitude() const;

    float GetMaxLatitude() const;

    void SetWorld(UWorld* InWorld)
    {
        WorldContext = InWorld;
    }

    UWorld* GetWorld() const
    {
        return WorldContext;
    }

private:
    /** Handles value changes from the slider. */
    void HandleSliderValueChanged(const TArray<float>& NewValues);

    UWorld* WorldContext = nullptr;

    float MinLatitude;
    float MaxLatitude;
    FSimpleDelegate OnLatitudeChanged;
};