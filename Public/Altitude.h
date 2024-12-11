#pragma once

#include "CoreMinimal.h"

/**
 * Calculate altitude based on pixel value.
 * @param PixelValue - The grayscale pixel value from the heightmap (0-255).
 * @param MinAltitude - The minimum altitude for the heightmap.
 * @param MaxAltitude - The maximum altitude for the heightmap.
 * @return Calculated altitude.
 */
float CalculateAltitude(uint8 PixelValue, float MinAltitude, float MaxAltitude);

/**
 * Calculate ocean depth based on sea level and raw altitude.
 * @param SeaLevel - The defined sea level.
 * @param RawAltitude - The raw altitude value.
 * @return Calculated ocean depth (negative if below sea level).
 */
float CalculateOceanDepth(float SeaLevel, float RawAltitude);
