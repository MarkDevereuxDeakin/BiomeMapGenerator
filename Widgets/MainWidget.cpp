#include "MainWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "ButtonRowWidget.h"

void SMainWidget::Construct(const FArguments& InArgs)
{    
    
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
                SAssignNew(DayLengthHoursTextBox, SEditableTextBox)
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
                SAssignNew(YearLengthDaysTextBox, SEditableTextBox)
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
                SAssignNew(DayOfYearTextBox, SEditableTextBox)
                .Text(FText::AsNumber(DayOfYear))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnDayOfYearChanged)
            ]
        ]

        // Northern Most Latitude Input
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
                .Text(FText::FromString("Northern Most Latitude (Degrees):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(NorthernLatitudeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(NorthernLatitude))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnNorthernLatitudeChanged)
            ]
        ]

        // Southern Most Latitude Input
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
                .Text(FText::FromString("Southern Most Latitude (Degrees):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(SouthernLatitudeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(SouthernLatitude))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnSouthernLatitudeChanged)
            ]
        ]

        // Maximum Altitude Input
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
                .Text(FText::FromString("Maximum Altitude (Metres):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(MaximumAltitudeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(MaximumAltitude))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnMaximumAltitudeChanged)
            ]
        ]

        // Maximum Altitude Input
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
                .Text(FText::FromString("Minimum Altitude (Metres):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(MinimumAltitudeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(MinimumAltitude))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnMinimumAltitudeChanged)
            ]
        ]

        // Sea Level Input
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
                .Text(FText::FromString("Sea Level (Metres):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(SeaLevelTextBox, SEditableTextBox)
                .Text(FText::AsNumber(SeaLevel))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnSeaLevelChanged)
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

void SMainWidget::OnNorthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        NorthernLatitude = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

void SMainWidget::OnSouthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        SouthernLatitude = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

void SMainWidget::OnMaximumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        MaximumAltitude = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

void SMainWidget::OnMinimumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        MinimumAltitude = FCString::Atof(*NewText.ToString());

        // Trigger callback
        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
}

void SMainWidget::OnSeaLevelChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        SeaLevel = FCString::Atof(*NewText.ToString());

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

float SMainWidget::GetNorthernLatitude() const
{
    return NorthernLatitude;
}

float SMainWidget::GetSouthernLatitude() const
{
    return SouthernLatitude;
}

float SMainWidget::GetMaximumAltitude() const
{
    return MaximumAltitude;
}

float SMainWidget::GetMinimumAltitude() const
{
    return MinimumAltitude;
}

float SMainWidget::GetSeaLevel() const
{
    return SeaLevel;
}