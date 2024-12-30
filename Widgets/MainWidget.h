// MainWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Main widget with EditableTextBoxes for Day Length (Hours) and Year Length (Days).
 */

// Delegate declarations

DECLARE_DELEGATE(FOnParametersChanged); // Add this delegate

class BIOMEMAPPER_API SMainWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMainWidget) {}
        SLATE_EVENT(FOnParametersChanged, OnParametersChanged)
        
    SLATE_END_ARGS()

    /** Constructs the Main Widget. */
    void Construct(const FArguments& InArgs);

    float GetDayLengthHours() const;
    float GetYearLengthDays() const;
    float GetDayOfYear() const;
    float GetNorthernLatitude() const;
    float GetSouthernLatitude() const;
    float GetMaximumAltitude() const;
    float GetMinimumAltitude() const;
    float GetSeaLevel() const;

private:
    
    float DayLengthHours = 24.0f;
    float YearLengthDays = 365.25;
    float DayOfYear = 0.0f;
    float SouthernLatitude = 25.0f;
    float NorthernLatitude = 35.0f;
    float MinimumAltitude = 0.0f;
    float MaximumAltitude = 2000.0f;
    float SeaLevel = 250.0f;
    
    FOnParametersChanged OnParametersChanged;

    TSharedPtr<SEditableTextBox> DayLengthHoursTextBox;
    TSharedPtr<SEditableTextBox> YearLengthDaysTextBox;
    TSharedPtr<SEditableTextBox> DayOfYearTextBox;
    TSharedPtr<SEditableTextBox> NorthernLatitudeTextBox;
    TSharedPtr<SEditableTextBox> SouthernLatitudeTextBox;
    TSharedPtr<SEditableTextBox> MaximumAltitudeTextBox;
    TSharedPtr<SEditableTextBox> MinimumAltitudeTextBox;
    TSharedPtr<SEditableTextBox> SeaLevelTextBox;

    /** Called when Day Length is updated. */
    void OnDayLengthChanged(const FText& NewText, ETextCommit::Type CommitType);

    /** Called when Year Length is updated. */
    void OnYearLengthChanged(const FText& NewText, ETextCommit::Type CommitType);  

    /**Called when Day of the year is updated */
    void OnDayOfYearChanged(const FText& NewText, ETextCommit::Type CommitType);

    /**Called when Northern Latitude is updated */
    void OnNorthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType);

    /**Called when Southern Latitude is updated */
    void OnSouthernLatitudeChanged(const FText& NewText, ETextCommit::Type CommitType);

    /**Called when Maximum Altitude is updated */
    void OnMaximumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType);

    /**Called when Minimum Altitude is updated */
    void OnMinimumAltitudeChanged(const FText& NewText, ETextCommit::Type CommitType);

    /**Called when Sea Level is updated */
    void OnSeaLevelChanged(const FText& NewText, ETextCommit::Type CommitType);
};