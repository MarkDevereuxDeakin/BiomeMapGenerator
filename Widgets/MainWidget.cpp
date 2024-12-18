#include "MainWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "ButtonRowWidget.h"

void SMainWidget::Construct(const FArguments& InArgs)
{
    DayLengthHours = InArgs._InitialDayLengthHours;
    YearLengthDays = InArgs._InitialYearLengthDays;
    DayOfYear = InArgs._InitialDayOfYear;
    OnParametersChanged = InArgs._OnParametersChanged;
    
    ChildSlot
    [
        SNew(SVerticalBox)

        // Day Length Input
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10) // Add some padding between inputs
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Day Length (Hours):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SNew(SEditableTextBox)
                .Text(FText::AsNumber(DayLengthHours))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnDayLengthChanged)
            ]
        ]

        // Year Length Input
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Year Length (Days):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SNew(SEditableTextBox)
                .Text(FText::AsNumber(YearLengthDays))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnYearLengthChanged)
            ]
        ]

        // Current Day Input
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Current Day of the Year:"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SNew(SEditableTextBox)
                .Text(FText::AsNumber(DayOfYear))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnDayOfYearChanged)
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

void SMainWidget::OnDayOfYearChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        DayOfYear = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

float SMainWidget::GetDayLengthHours() const
{
    return DayLengthHours;
}

float SMainWidget::GetYearLengthDays() const
{
    return YearLengthDays;
}

float SMainWidget::GetDayOfYear() const
{
    return DayOfYear;
}
