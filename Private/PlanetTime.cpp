#include "PlanetTime.h"

// Static Instance Initialization
FPlanetTime* FPlanetTime::Instance = nullptr;

// Private Constructors
FPlanetTime::FPlanetTime()
    : YearLength(365.0f), DayLengthHours(24.0f), DayLengthMinutes(0.0f), DayLengthSeconds(86400.0f), DayOfYear(1), TimeOfDay(0.0f)
{
}

FPlanetTime::FPlanetTime(float YearLengthDays, float DayLengthHours, float DayLengthMinutes, int32 DayOfYear, float TimeOfDay)
    : YearLength(YearLengthDays),
      DayLengthHours(DayLengthHours),
      DayLengthMinutes(DayLengthMinutes),
      DayLengthSeconds((DayLengthHours * 3600.0f) + (DayLengthMinutes * 60.0f)),
      DayOfYear(DayOfYear),
      TimeOfDay(TimeOfDay)
{
}

// Singleton Access
FPlanetTime& FPlanetTime::GetInstance()
{
    ensure(Instance != nullptr); // Ensure it is initialized before use
    return *Instance;
}

// Initialize Singleton
void FPlanetTime::Initialize(float YearLengthDays, float DayLengthHours, float DayLengthMinutes, int32 DayOfYear, float TimeOfDay)
{
    if (Instance == nullptr)
    {
        Instance = new FPlanetTime(YearLengthDays, DayLengthHours, DayLengthMinutes, DayOfYear, TimeOfDay);
    }
}
// Getters
float FPlanetTime::GetYearLength() const { return YearLength; }
float FPlanetTime::GetDayLengthHours() const { return DayLengthHours; }
float FPlanetTime::GetDayLengthMinutes() const { return DayLengthMinutes; }
float FPlanetTime::GetDayLengthSeconds() const { return DayLengthSeconds; }
int32 FPlanetTime::GetDayOfYear() const { return DayOfYear; }
float FPlanetTime::GetTimeOfDay() const { return TimeOfDay; }

// Setters
void FPlanetTime::SetYearLength(float NewYearLengthDays)
{ 
    YearLength = NewYearLengthDays + 1;
}

void FPlanetTime::SetDayLengthHours(float NewDayLengthHours)
{
    this->DayLengthHours = NewDayLengthHours;
    DayLengthSeconds = (NewDayLengthHours * 3600.0f) + (DayLengthMinutes * 60.0f);
}
void FPlanetTime::SetDayLengthMinutes(float NewDayLengthMinutes)
{
    this->DayLengthMinutes = NewDayLengthMinutes;
    DayLengthSeconds = (DayLengthHours * 3600.0f) + (NewDayLengthMinutes * 60.0f);
}
void FPlanetTime::SetDayOfYear(int32 NewDayOfYear)
{ 
    this->DayOfYear = NewDayOfYear; 
}
void FPlanetTime::SetTimeOfDay(float NewTimeOfDay)
{ 
    this->TimeOfDay = FMath::Clamp(NewTimeOfDay, 0.0f, 1.0f); 
}

// Determine Season
FString FPlanetTime::GetSeason(float TimeOfYear) const
{
    if (TimeOfYear < 0.25f)
    {
        return TEXT("Spring");
    }
    else if (TimeOfYear < 0.50f)
    {
        return TEXT("Summer");
    }
    else if (TimeOfYear < 0.75f)
    {
        return TEXT("Autumn");
    }
    else
    {
        return TEXT("Winter");
    }
}
