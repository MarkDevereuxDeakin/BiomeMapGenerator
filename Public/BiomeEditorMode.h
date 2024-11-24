#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

class BiomeEditorToolkit;

/**
 * Custom editor mode for biome editing.
 */
class FBiomeEditorMode : public FEdMode
{
public:
    static const FEditorModeID EM_BiomeEditorMode;

    FBiomeEditorMode();
    virtual ~FBiomeEditorMode();

    // Called when entering the mode
    virtual void Enter() override;

    // Called when exiting the mode
    virtual void Exit() override;    

    // Determines if the mode uses a toolkit for UI
    virtual bool UsesToolkits() const override;

private:
   // Method to spawn the tab content
    TSharedRef<SDockTab> SpawnBiomeTab(const FSpawnTabArgs& Args);

    // Callback for when the tab is closed
    void OnTabClosed(TSharedRef<SDockTab> ClosedTab);

    // Pointer to the widget content for the Biome Editor
    TSharedPtr<SWidget> ToolkitWidget;

    // Static member to track the current tab instance
    static TWeakPtr<SDockTab> BiomeTabInstance;
};
