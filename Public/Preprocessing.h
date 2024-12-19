#pragma once

#include "CoreMinimal.h"
#include "HeightmapCell.h"

class Preprocessing
{
public:
    static bool PreprocessData(TArray<FHeightmapCell>& HeightmapData, int32 Width, int32 Height);
};
