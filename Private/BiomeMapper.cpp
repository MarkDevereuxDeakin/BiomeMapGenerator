#include "BiomeMapper.h"
#include "PlanetTime.h"
#include "BiomeEditorMode.h"
#include "EditorModeRegistry.h"
#include "Editor/EditorEngine.h" // Ensure this is included for editor functionality

#define LOCTEXT_NAMESPACE "FBiomeMapperModule"

void FBiomeMapperModule::StartupModule()
{
    // Register the Biome Editor Mode
    FEditorModeRegistry::Get().RegisterMode<FBiomeEditorMode>(
        FBiomeEditorMode::EM_BiomeEditorMode,
        LOCTEXT("BiomeEditorModeName", "Biome Mapper"),
        FSlateIcon(), // Use a default or replace with your own icon
        true
    );
}

void FBiomeMapperModule::ShutdownModule()
{
    // Unregister the Biome Editor Mode
    FEditorModeRegistry::Get().UnregisterMode(FBiomeEditorMode::EM_BiomeEditorMode);

    UE_LOG(LogTemp, Log, TEXT("BiomeMapper Module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBiomeMapperModule, BiomeMapper)
