// MultiHandleSlider.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Rendering/DrawElements.h"

DECLARE_DELEGATE_OneParam(FOnMultiHandleSliderValueChanged, const TArray<float>&);

/**
 * A custom slider with support for multiple handles.
 */
class BIOMEMAPPER_API SMultiHandleSlider : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMultiHandleSlider) {}
        SLATE_ATTRIBUTE(float, MinValue)
        SLATE_ATTRIBUTE(float, MaxValue)
        SLATE_ATTRIBUTE(TArray<float>, HandleValues)
        SLATE_ATTRIBUTE(float, SliderInterval) // Add interval
        SLATE_EVENT(FOnMultiHandleSliderValueChanged, OnValueChanged)
        SLATE_ATTRIBUTE(TFunction<FString(float)>, LabelFormatter) // Add label formatting
    SLATE_END_ARGS()


    /** Construct the widget */
    void Construct(const FArguments& InArgs);

    // Get the current handle values
    TArray<float> GetHandleValues() const;

private:
    /** Minimum and maximum values for the slider */
    TAttribute<float> MinValue;
    TAttribute<float> MaxValue;

    /** Array of values for the handles */
    FOnMultiHandleSliderValueChanged OnValueChanged;

    /** Array of values for the handles */
    TAttribute<TArray<float>> HandleValues;

        /** Interval between scale marks */
    TAttribute<float> SliderInterval;

    /** Function to format the labels */
    TFunction<FString(float)> LabelFormatter; // Directly define as TFunction


    /** Draws the slider handles */
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                          int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    /** Handles mouse input for interaction */
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    /** Tracks which handle is being dragged */
    int32 ActiveHandleIndex;

    /** Helper function to draw the slider bar */
    void DrawSliderBar(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;

    /** Helper function to draw handles */
    void DrawHandles(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;

    /** Helper function to calculate handle positions */
    float GetHandlePosition(float Value, const FGeometry& AllottedGeometry) const;

    /** Helper function to update handle values based on mouse position */
    void UpdateHandleValueFromMouse(const FGeometry& MyGeometry, const FVector2D& MousePosition);
};
