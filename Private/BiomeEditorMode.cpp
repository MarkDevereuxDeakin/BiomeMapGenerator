#include "BiomeEditorMode.h"
#include "BiomeEditorToolkit.h"
#include "EditorModeManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Editor/UnrealEd/Public/Toolkits/BaseToolkit.h"

const FEditorModeID FBiomeEditorMode::EM_BiomeEditorMode = TEXT("EM_BiomeEditorMode");

// Add a member to track the tab
TWeakPtr<SDockTab> FBiomeEditorMode::BiomeTabInstance = nullptr;

FBiomeEditorMode::FBiomeEditorMode()
{
}

FBiomeEditorMode::~FBiomeEditorMode()
{
}
void FBiomeEditorMode::Enter()
{
    FEdMode::Enter();

    // Define a unique tab name
    const FName TabName = FName("BiomeEditor");

    // Check if the tab is already open
    TSharedPtr<SDockTab> ExistingTab = FGlobalTabmanager::Get()->FindExistingLiveTab(TabName);

    if (ExistingTab.IsValid())
    {
        // Bring the existing tab to the foreground
        ExistingTab->ActivateInParent(ETabActivationCause::SetDirectly);
        BiomeTabInstance = ExistingTab;
        return;
    }

    // Create the Toolkit Widget if not already created
   if (!ToolkitWidget.IsValid())
    {
        ToolkitWidget = SNew(BiomeEditorToolkit);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ToolkitWidget is already valid."));
    }

    // Register the tab if it hasn't been registered
    if (!FGlobalTabmanager::Get()->HasTabSpawner(TabName))
    {
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName,
            FOnSpawnTab::CreateRaw(this, &FBiomeEditorMode::SpawnBiomeTab))
            .SetDisplayName(FText::FromString("Biome Editor"))
            .SetIcon(FSlateIcon());
    }

    // Open the tab
    TSharedPtr<SDockTab> NewTab = FGlobalTabmanager::Get()->TryInvokeTab(TabName);
    if (NewTab.IsValid())
    {
        BiomeTabInstance = NewTab; // Track the new tab instance
    }
}


void FBiomeEditorMode::Exit()
{
    if (BiomeTabInstance.IsValid())
    {
        TSharedPtr<SDockTab> ExistingTab = BiomeTabInstance.Pin();
        if (ExistingTab.IsValid())
        {
            ExistingTab->RequestCloseTab();
        }
        BiomeTabInstance.Reset();
    }

    if (ToolkitWidget.IsValid())
    {
        ToolkitWidget.Reset();
    }

    FEdMode::Exit();
}


void FBiomeEditorMode::OnTabClosed(TSharedRef<SDockTab> ClosedTab)
{
    if (BiomeTabInstance.IsValid() && BiomeTabInstance.Pin() == ClosedTab)
    {
        BiomeTabInstance.Reset();
    }
}

TSharedRef<SDockTab> FBiomeEditorMode::SpawnBiomeTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(FText::FromString("Biome Editor"))
        .OnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FBiomeEditorMode::OnTabClosed))
        [
            ToolkitWidget.ToSharedRef() // Add the ToolkitWidget as the tab's content
        ];
}



bool FBiomeEditorMode::UsesToolkits() const
{
    return false; // Returning false as we are embedding the widget directly
}
