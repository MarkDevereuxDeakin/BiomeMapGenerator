// MainWidget.cpp
#include "MainWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"


void SMainWidget::Construct(const FArguments& InArgs)
{
    DayLengthHours = InArgs._InitialDayLengthHours;
    YearLengthDays = InArgs._InitialYearLengthDays;
    OnParametersChanged = InArgs._OnParametersChanged;

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Day Length (Hours):"))
                .Justification(ETextJustify::Right)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SEditableTextBox)
                .Text(FText::AsNumber(DayLengthHours))
                .OnTextCommitted(this, &SMainWidget::OnDayLengthChanged)
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Year Length (Days):"))
                .Justification(ETextJustify::Right)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SEditableTextBox)
                .Text(FText::AsNumber(YearLengthDays))
                .OnTextCommitted(this, &SMainWidget::OnYearLengthChanged)
            ]
        ]
    ];
}

void SMainWidget::OnDayLengthChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        DayLengthHours = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

void SMainWidget::OnYearLengthChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        YearLengthDays = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}