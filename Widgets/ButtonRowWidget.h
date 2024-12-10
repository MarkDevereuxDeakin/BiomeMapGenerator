// ButtonRowWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * A widget with buttons for uploading a heightmap and calculating biomes.
 */
class BIOMEMAPPER_API SButtonRowWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SButtonRowWidget) {}
        SLATE_EVENT(FSimpleDelegate, OnUploadHeightmap)
        SLATE_EVENT(FSimpleDelegate, OnCalculateBiome)
    SLATE_END_ARGS()

    /** Constructs the Button Row Widget. */
    void Construct(const FArguments& InArgs);

private:
    FSimpleDelegate OnUploadHeightmap;
    FSimpleDelegate OnCalculateBiome;

    /** Called when the Upload button is clicked. */
    FReply HandleUploadClicked();

    /** Called when the Calculate Biome button is clicked. */
    FReply HandleCalculationClicked();
};