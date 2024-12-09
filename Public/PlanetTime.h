#pragma once

#include "CoreMinimal.h"
#include "PlanetTime.generated.h"

/**
 * Struct to represent planetary time.
 * Provides functionality for advancing time and retrieving time-related information.
 */
USTRUCT(BlueprintType)
struct BIOMEMAPPER_API FPlanetTime
{
    GENERATED_BODY()

public:
    /** Default constructor */
    FPlanetTime()
    : DayLengthSeconds(86400.0f),  // Default to 24 hours in seconds
      YearLengthDays(365.25f),     // Default to Earth's year length in days
      MonthsPerYear(12),           // Default to 12 months
      CurrentTimeSeconds(0.0f),    // Start at time zero
      YearCount(0)                 // Start at year zero;
      {
        
      }

    /**
     * Parameterized constructor
     * @param InDayLengthHours - Length of a day in hours
     * @param InYearLengthDays - Length of a year in days
     * @param InMonthsPerYear - Number of months in a year (default: 12)
     */
    FPlanetTime(float InDayLengthHours, float InYearLengthDays, int32 InMonthsPerYear = 12);

    /**
     * Advance the planetary time by a given delta in seconds.
     * @param DeltaSeconds - Time increment in seconds
     */
    void AdvanceTime(float DeltaSeconds);

    /**
     * Get the current year.
     * @return The current year (1-based).
     */
    int32 GetYear() const;

    /**
     * Get the current day of the year.
     * @return The current day of the year (1-based).
     */
    int32 GetDayOfYear() const;

    /**
     * Get the current time of day in seconds.
     * @return Time of day in seconds within the current day.
     */
    float GetTimeOfDay() const;

    /**
     * Get the current month and day within the month.
     * @param OutMonth - Output parameter for the current month (1-based).
     * @param OutDay - Output parameter for the current day within the month (1-based).
     */
    void GetMonthAndDay(int32& OutMonth, int32& OutDay) const;

    /**
     * Get a formatted string representing the current planetary time.
     * @return A string in the format "Year: X, Month: X, Day: X, Time: X hours".
     */
    FString GetFormattedTime() const;

    /**
     * Get the length of a day in seconds.
     * @return Length of a day in seconds.
     */
    float GetDayLengthSeconds() const;

    /**
     * Get the length of a year in days.
     * @return Length of a year in days.
     */
    float GetYearLengthDays() const { return YearLengthDays; }

private:
    /** Length of a day in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float DayLengthSeconds;

    /** Length of a year in days */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float YearLengthDays;

    /** Number of months in a year */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    int32 MonthsPerYear;

    /** Current simulation time in seconds since the start */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float CurrentTimeSeconds;

    /** Number of completed years */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    int32 YearCount;

    /**
     * Calculate the number of days in a month.
     * @return Days per month.
     */
    int32 GetDaysPerMonth() const;
};
