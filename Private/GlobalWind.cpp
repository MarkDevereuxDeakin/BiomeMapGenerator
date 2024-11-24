#include "GlobalWind.h"

FVector2D GlobalWind::CalculateWindDirection(float Latitude)
{
    if (Latitude >= -30.0f && Latitude <= 30.0f)
    {
        // Trade winds: Blow toward the equator (east to west)
        return FVector2D(-1.0f, 0.0f); // Westward
    }
    else if ((Latitude > 30.0f && Latitude <= 60.0f) || (Latitude < -30.0f && Latitude >= -60.0f))
    {
        // Westerlies: Blow toward the poles (west to east)
        return FVector2D(1.0f, 0.0f); // Eastward
    }
    else if ((Latitude > 60.0f && Latitude <= 90.0f) || (Latitude < -60.0f && Latitude >= -90.0f))
    {
        // Polar easterlies: Blow toward the equator (east to west)
        return FVector2D(-1.0f, 0.0f); // Westward
    }
    else
    {
        // Default to no wind if latitude is out of range
        return FVector2D(0.0f, 0.0f);
    }
}
