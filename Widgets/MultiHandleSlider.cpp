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
    FVector2D StartPoint(AllottedGeometry.GetLocalSize().X / 2, 0.0f);
    FVector2D EndPoint(AllottedGeometry.GetLocalSize().X / 2, AllottedGeometry.GetLocalSize().Y);

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
        FVector2D HandlePosition(AllottedGeometry.GetLocalSize().X / 2 - 5.0f, Position - 5.0f);

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(HandlePosition, FVector2D(10.0f, 10.0f)),
            FCoreStyle::Get().GetBrush("GenericWhiteBox"),
            ESlateDrawEffect::None,
            (i == 0 || i == Values.Num() - 1) ? FLinearColor::Gray : FLinearColor::Blue
        );
    }
}

float SMultiHandleSlider::GetHandlePosition(float Value, const FGeometry& AllottedGeometry) const
{
    float Range = MaxValue.Get() - MinValue.Get();
    float NormalizedValue = (Value - MinValue.Get()) / Range;
    return AllottedGeometry.GetLocalSize().Y * (1.0f - NormalizedValue); // Invert for vertical orientation
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
