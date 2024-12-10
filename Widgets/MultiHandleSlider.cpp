#include "MultiHandleSlider.h"
#include "Rendering/DrawElements.h"

void SMultiHandleSlider::Construct(const FArguments& InArgs)
{
    MinValue = InArgs._MinValue;
    MaxValue = InArgs._MaxValue;
    HandleValues = InArgs._HandleValues;
    OnValueChanged = InArgs._OnValueChanged;

    ActiveHandleIndex = INDEX_NONE;
}

TArray<float> SMultiHandleSlider::GetHandleValues() const
{
    return HandleValues.Get();
}

int32 SMultiHandleSlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                  const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                  int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    DrawSliderBar(AllottedGeometry, OutDrawElements, LayerId);
    DrawHandles(AllottedGeometry, OutDrawElements, LayerId);

    return LayerId + 1;
}

void SMultiHandleSlider::DrawSliderBar(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    float Padding = 10.0f; // Adjust the padding value as needed

    FVector2D StartPoint(AllottedGeometry.GetLocalSize().X / 2, Padding);
    FVector2D EndPoint(AllottedGeometry.GetLocalSize().X / 2, AllottedGeometry.GetLocalSize().Y - Padding);

    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        {StartPoint, EndPoint},
        ESlateDrawEffect::None,
        FLinearColor::Gray
    );
}

void SMultiHandleSlider::DrawHandles(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    const TArray<float>& Values = HandleValues.Get();

    for (int32 i = 0; i < Values.Num(); ++i)
    {
        float Position = GetHandlePosition(Values[i], AllottedGeometry);

        FVector2D HandlePosition;
        FVector2D HandleSize;

        // Define horizontal bar-like handles
        if (i == 2) // Sea Level: Blue horizontal bar
        {
            HandlePosition = FVector2D(AllottedGeometry.GetLocalSize().X / 2 - 10.0f, Position - 2.0f);
            HandleSize = FVector2D(20.0f, 4.0f); // Slim horizontal bar
        }
        else // Min/Max Altitude or Latitude: Grey bars
        {
            HandlePosition = FVector2D(AllottedGeometry.GetLocalSize().X / 2 - 7.0f, Position - 2.0f);
            HandleSize = FVector2D(14.0f, 4.0f); // Slim horizontal bar
        }

        // Determine handle color
        FLinearColor HandleColor = (i == 2) ? FLinearColor::Blue : FLinearColor::Gray;

        // Draw the bar-like handle
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(HandlePosition, HandleSize),
            FCoreStyle::Get().GetBrush("GenericWhiteBox"),
            ESlateDrawEffect::None,
            HandleColor
        );
    }
}


float SMultiHandleSlider::GetHandlePosition(float Value, const FGeometry& AllottedGeometry) const
{
    float Range = MaxValue.Get() - MinValue.Get();
    float NormalizedValue = (Value - MinValue.Get()) / Range;

    // Adjust handle position to respect the slider bar's padding
    float Padding = 10.0f; // Match the padding in DrawSliderBar
    float BarHeight = AllottedGeometry.GetLocalSize().Y - (2 * Padding);

    return Padding + (1.0f - NormalizedValue) * BarHeight; // Invert for vertical orientation
}

FReply SMultiHandleSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    const TArray<float>& Values = HandleValues.Get();

    for (int32 i = 0; i < Values.Num(); ++i)
    {
        float HandlePosition = GetHandlePosition(Values[i], MyGeometry);
        if (FMath::Abs(LocalMousePosition.Y - HandlePosition) < 10.0f) // Handle size tolerance
        {
            ActiveHandleIndex = i;
            return FReply::Handled().CaptureMouse(SharedThis(this));
        }
    }

    return FReply::Unhandled();
}

FReply SMultiHandleSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ActiveHandleIndex != INDEX_NONE)
    {
        UpdateHandleValueFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition());
        OnValueChanged.ExecuteIfBound(HandleValues.Get());
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SMultiHandleSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ActiveHandleIndex != INDEX_NONE)
    {
        ActiveHandleIndex = INDEX_NONE;
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

void SMultiHandleSlider::UpdateHandleValueFromMouse(const FGeometry& MyGeometry, const FVector2D& MousePosition)
{
    FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MousePosition);
    float Range = MaxValue.Get() - MinValue.Get();
    float NewValue = MinValue.Get() + ((1.0f - LocalMousePosition.Y / MyGeometry.GetLocalSize().Y) * Range);

    NewValue = FMath::Clamp(NewValue, MinValue.Get(), MaxValue.Get());

    TArray<float> Values = HandleValues.Get();
    Values[ActiveHandleIndex] = NewValue;
    HandleValues.Set(Values);
}
