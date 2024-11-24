#pragma once

#include "CoreMinimal.h"

float CalculateAltitude(uint8 PixelValue, float MinAltitude, float MaxAltitude);
float CalculateOceanDepth(float SeaLevel, float RawAltitude);
