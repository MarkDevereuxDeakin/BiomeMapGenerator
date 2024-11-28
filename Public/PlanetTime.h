#pragma once

#include "CoreMinimal.h"
#include "PlanetTime.generated.h" // Required for Unreal's reflection system

USTRUCT(BlueprintType)
struct BIOMEMAPPER_API FPlanetTime
{
    GENERATED_BODY()

public:
    // Default constructor
    FPlanetTime()
        : DayLengthSeconds(86400.0f), YearLengthDays(365.25f), MonthsPerYear(12), CurrentTimeSeconds(0.0f), YearCount(0)
    {}

    // Constructor with parameters
    FPlanetTime(float InDayLengthSeconds, float InYearLengthDays, int32 InMonthsPerYear = 12);

    // Advance time by a given delta in seconds
    void AdvanceTime(float DeltaSeconds);

    // Get the current year
    int32 GetYear() const;

    // Get the current day in the year (1-based)
    int32 GetDayOfYear() const;

    // Get the current time of day in seconds
    float GetTimeOfDay() const;

    // Get the current month and day within the month
    void GetMonthAndDay(int32& OutMonth, int32& OutDay) const;

    // Debugging: Get a formatted string representing the current planetary time
    FString GetFormattedTime() const;

    // Getter for DayLengthSeconds
    float GetDayLengthSeconds() const;

    float GetYearLengthDays() const { return YearLengthDays; }

private:
    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float DayLengthSeconds; // Length of a day in seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float YearLengthDays;   // Length of a year in days

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    int32 MonthsPerYear;    // Number of months in a year

    // Internal time tracking
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    float CurrentTimeSeconds; // Time in seconds since the start of the simulation

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlanetTime", meta = (AllowPrivateAccess = "true"))
    int32 YearCount;          // Completed years

    // Helper: Calculate days per month
    int32 GetDaysPerMonth() const;
};
