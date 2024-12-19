#pragma once

#include "CoreMinimal.h"

/**
 * A Singleton struct to represent planetary time.
 * Provides functionality for advancing time and retrieving time-related information.
 */

struct BIOMEMAPPER_API FPlanetTime
{

public:
    // Singleton Access
    static FPlanetTime& GetInstance();

    // Initialize the Singleton
    static void Initialize(float YearLengthDays, float DayLengthHours, float DayLengthMinutes, int32 DayOfYear, float TimeOfDay);

    // Getters
    float GetYearLength() const;
    float GetDayLengthHours() const;
    float GetDayLengthMinutes() const;
    float GetDayLengthSeconds() const;
    int32 GetDayOfYear() const;
    float GetTimeOfDay() const;
    FString GetSeason(float TimeOfYear) const;

    // Setters
    void SetYearLength(float NewYearLengthDays);
    void SetDayLengthHours(float NewDayLengthHours);
    void SetDayLengthMinutes(float NewDayLengthMinutes);
    void SetDayOfYear(int32 NewDayOfYear);
    void SetTimeOfDay(float NewTimeOfDay);

private:
    // Private Constructor for Singleton
    FPlanetTime();
    FPlanetTime(float YearLengthDays, float DayLengthHours, float DayLengthMinutes, int32 DayOfYear, float TimeOfDay);

    // Static Instance
    static FPlanetTime* Instance;

    // Internal Data Members
    float YearLength;          // Length of the year in days
    float DayLengthHours;      // Length of the day in hours
    float DayLengthMinutes;    // Length of the day in minutes
    float DayLengthSeconds;    // Length of the day in seconds
    int32 DayOfYear;           // Current day of the year
    float TimeOfDay;        

};