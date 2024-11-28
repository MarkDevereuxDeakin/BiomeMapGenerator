#include "PlanetTime.h"

FPlanetTime::FPlanetTime(float InDayLengthSeconds, float InYearLengthDays, int32 InMonthsPerYear)
    : DayLengthSeconds(InDayLengthSeconds),
      YearLengthDays(InYearLengthDays),
      MonthsPerYear(InMonthsPerYear),
      CurrentTimeSeconds(0.0f),
      YearCount(0)
{
    // Validate inputs
    ensure(this->DayLengthSeconds > 0.0f);
    ensure(this->YearLengthDays > 0.0f);
    ensure(this->MonthsPerYear > 0);
}

void FPlanetTime::AdvanceTime(float DeltaSeconds)
{
    this->CurrentTimeSeconds += DeltaSeconds;

    // Check if a year has completed
    float YearLengthInSeconds = this->YearLengthDays * this->DayLengthSeconds;
    if (this->CurrentTimeSeconds >= YearLengthInSeconds)
    {
        this->CurrentTimeSeconds -= YearLengthInSeconds;
        this->YearCount++;
    }
}

int32 FPlanetTime::GetYear() const
{
    return this->YearCount + 1; // 1-based year
}

int32 FPlanetTime::GetDayOfYear() const
{
    return FMath::FloorToInt(this->CurrentTimeSeconds / this->DayLengthSeconds) + 1; // 1-based day of year
}

float FPlanetTime::GetTimeOfDay() const
{
    return FMath::Fmod(this->CurrentTimeSeconds, this->DayLengthSeconds); // Time within the current day
}

float FPlanetTime::GetDayLengthSeconds() const
{
    return this->DayLengthSeconds;
}

void FPlanetTime::GetMonthAndDay(int32& OutMonth, int32& OutDay) const
{
    int32 TotalDay = this->GetDayOfYear(); // 1-based day of year
    int32 DaysPerMonth = this->GetDaysPerMonth();

    OutMonth = (TotalDay - 1) / DaysPerMonth + 1; // 1-based month
    OutDay = (TotalDay - 1) % DaysPerMonth + 1;   // 1-based day within the month
}

FString FPlanetTime::GetFormattedTime() const
{
    int32 CurrentYear = this->GetYear();
    int32 CurrentDay = this->GetDayOfYear();
    float TimeOfDay = this->GetTimeOfDay();

    int32 CurrentMonth, DayOfMonth;
    this->GetMonthAndDay(CurrentMonth, DayOfMonth);

    return FString::Printf(TEXT("Year: %d, Month: %d, Day: %d, Time: %.2f hours"),
                           CurrentYear, CurrentMonth, DayOfMonth, TimeOfDay / 3600.0f);
}

int32 FPlanetTime::GetDaysPerMonth() const
{
    return FMath::CeilToInt(this->YearLengthDays / this->MonthsPerYear);
}
