// MainWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Main widget with EditableTextBoxes for Day Length (Hours) and Year Length (Days).
 */
class BIOMEMAPPER_API SMainWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMainWidget) {}
        SLATE_ARGUMENT(float, InitialDayLengthHours)
        SLATE_ARGUMENT(float, InitialYearLengthDays)
        SLATE_EVENT(FSimpleDelegate, OnParametersChanged)
    SLATE_END_ARGS()

    /** Constructs the Main Widget. */
    void Construct(const FArguments& InArgs);

private:
    float DayLengthHours;
    float YearLengthDays;
    FSimpleDelegate OnParametersChanged;

    /** Called when Day Length is updated. */
    void OnDayLengthChanged(const FText& NewText, ETextCommit::Type CommitType);

    /** Called when Year Length is updated. */
    void OnYearLengthChanged(const FText& NewText, ETextCommit::Type CommitType);
};
