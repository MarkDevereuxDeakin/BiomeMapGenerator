// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BiomeMapper : ModuleRules
{
	public BiomeMapper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"BiomeMapper/Widgets",
                "BiomeMapper/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"BiomeMapper/Widgets",
                "BiomeMapper/Private"
			}
			);
			
		
		// Public dependencies
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",      // Required for Slate widgets
            "SlateCore",   // Required for Slate widgets
            "BiomeMapper",
            "ImageWrapper"
        });

        // Private dependencies (Editor-only modules)
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",      // Editor utilities and extensions
            "LevelEditor",       // Provides editor-related functionality
            "EditorFramework", // For FEditorModeInfo
            "EditorStyle",   // Optional: For editor-specific styling
            "Projects",      // For working with projects in the editor
            "ToolMenus",      // For adding custom menus in the editor
            "ImageWrapper"
        });		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

			 // Ensure UnrealEd is only included for editor builds
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.Add("EditorScriptingUtilities");
        }

        // Enable IWYU for better header management
        bEnforceIWYU = true;

        // Optimize build times
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bUseUnity = true;
	}
}
