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
                .ToolTipText(FText::FromString("Enter a value between 0 and Year Length"))
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
                .ToolTipText(FText::FromString("Enter a value between -90 and 90 degrees"))
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
                .ToolTipText(FText::FromString("Enter a value between -90 and 90 degrees"))
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
                .Text(FText::FromString("Central Longitude for Heightmap (Degrees):"))
                .Justification(ETextJustify::Left)
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(10, 0)
            [
                SAssignNew(CentralLongitudeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(CentralLongitude))
                .Justification(ETextJustify::Right)
                .OnTextCommitted(this, &SMainWidget::OnCentralLongitudeChanged)
                .ToolTipText(FText::FromString("Enter a value between -180 and 180 degrees"))
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
                .ToolTipText(FText::FromString("Enter a Maximum Heightmap Height in metres"))
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
                .ToolTipText(FText::FromString("Enter a Minimum Heightmap Height in metres"))
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
                .ToolTipText(FText::FromString("Enter a Sea Level Height in metres"))
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
        float NewDayOfYear = FCString::Atof(*NewText.ToString());

        // Validate range based on YearLengthDays
        if (NewDayOfYear < 1.0f || NewDayOfYear > YearLengthDays)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Day of Year: %f. It must be between 1 and %f."), NewDayOfYear, YearLengthDays);
            DayOfYearTextBox->SetText(FText::AsNumber(DayOfYear)); // Restore previous value
            return;
        }

        DayOfYear = NewDayOfYear;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Day of Year. Must be numeric."));
    }
}


void SMainWidget::OnNorthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewLatitude = FCString::Atof(*NewText.ToString());

        // Validate range
        if (NewLatitude < -90.0f || NewLatitude > 90.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Northern Latitude: %f. It must be between -90 and 90 degrees."), NewLatitude);
            NorthernLatitudeTextBox->SetText(FText::AsNumber(NorthernLatitude)); // Restore previous value
            return;
        }

        NorthernLatitude = NewLatitude;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Northern Latitude. Must be numeric."));
    }
}

void SMainWidget::OnSouthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewLatitude = FCString::Atof(*NewText.ToString());

        // Validate range
        if (NewLatitude < -90.0f || NewLatitude > 90.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Southern Latitude: %f. It must be between -90 and 90 degrees."), NewLatitude);
            SouthernLatitudeTextBox->SetText(FText::AsNumber(SouthernLatitude)); // Restore previous value
            return;
        }

        SouthernLatitude = NewLatitude;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Southern Latitude. Must be numeric."));
    }
}

void SMainWidget::OnCentralLongitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewLongitude = FCString::Atof(*NewText.ToString());

        // Validate range
        if (NewLongitude < -180.0f || NewLongitude > 180.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Central Longitude: %f. It must be between -180 and 180 degrees."), NewLongitude);
            CentralLongitudeTextBox->SetText(FText::AsNumber(CentralLongitude)); // Restore previous value
            return;
        }

        CentralLongitude = NewLongitude;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Central Longitude. Must be numeric."));
    }
}

void SMainWidget::OnMaximumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewMaxAltitude = FCString::Atof(*NewText.ToString());

        // Validate range: MaximumAltitude should be greater than or equal to MinimumAltitude
        if (NewMaxAltitude < MinimumAltitude)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Maximum Altitude: %f. It must be greater than or equal to Minimum Altitude: %f."),
                NewMaxAltitude, MinimumAltitude);
            MaximumAltitudeTextBox->SetText(FText::AsNumber(MaximumAltitude)); // Restore previous value
            return;
        }

        MaximumAltitude = NewMaxAltitude;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Maximum Altitude. Must be numeric."));
    }
}


void SMainWidget::OnMinimumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewMinAltitude = FCString::Atof(*NewText.ToString());

        // Validate range: MinimumAltitude should be less than or equal to MaximumAltitude
        if (NewMinAltitude > MaximumAltitude)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Minimum Altitude: %f. It must be less than or equal to Maximum Altitude: %f."),
                NewMinAltitude, MaximumAltitude);
            MinimumAltitudeTextBox->SetText(FText::AsNumber(MinimumAltitude)); // Restore previous value
            return;
        }

        MinimumAltitude = NewMinAltitude;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Minimum Altitude. Must be numeric."));
    }
}


void SMainWidget::OnSeaLevelChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    if (NewText.IsNumeric())
    {
        float NewSeaLevel = FCString::Atof(*NewText.ToString());

        // Validate range based on MinimumAltitude and MaximumAltitude
        if (NewSeaLevel < MinimumAltitude || NewSeaLevel > MaximumAltitude)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Sea Level: %f. It must be between %f (Minimum Altitude) and %f (Maximum Altitude)."), 
                NewSeaLevel, MinimumAltitude, MaximumAltitude);
            SeaLevelTextBox->SetText(FText::AsNumber(SeaLevel)); // Restore previous value
            return;
        }

        SeaLevel = NewSeaLevel;

        if (OnParametersChanged.IsBound())
        {
            OnParametersChanged.Execute();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input for Sea Level. Must be numeric."));
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

float SMainWidget::GetCentralLongitude() const
{
    return CentralLongitude;
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