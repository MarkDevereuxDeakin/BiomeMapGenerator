#include "Altitude.h"

float CalculateAltitude(uint8 PixelValue, float MinAltitude, float MaxAltitude)
{
    return MinAltitude + (PixelValue / 255.0f) * (MaxAltitude - MinAltitude);
}

float CalculateOceanDepth(float SeaLevel, float RawAltitude)
{
    return SeaLevel - RawAltitude;
}
