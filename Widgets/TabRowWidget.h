#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * A widget with buttons for uploading a heightmap and calculating biomes.
 */
class BIOMEMAPPER_API STabRowWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STabRowWidget) {}
        SLATE_EVENT(FSimpleDelegate, OnShowHeightmap)
        SLATE_EVENT(FSimpleDelegate, OnShowBiomeMap)
    SLATE_END_ARGS()

    /** Constructs the Button Tab Widget. */
    void Construct(const FArguments& InArgs);

private:
    FSimpleDelegate OnShowHeightmap;
    FSimpleDelegate OnShowBiomeMap;

    /** Called when the Heightmap button is clicked. */
    FReply HandleHeightmapClicked();

    /** Called when the Biome Map button is clicked. */
    FReply HandleBiomeMapClicked();
};