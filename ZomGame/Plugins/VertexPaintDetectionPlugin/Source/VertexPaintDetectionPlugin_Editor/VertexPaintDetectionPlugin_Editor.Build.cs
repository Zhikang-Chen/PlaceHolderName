// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

using UnrealBuildTool;

public class VertexPaintDetectionPlugin_Editor : ModuleRules
{
	public VertexPaintDetectionPlugin_Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		//if (Target.bBuildEditor)
		//{
		//	bUseUnity = false;
		//}

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
            }
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"InputCore",
				"RenderCore",
				"UnrealEd",
				"EditorSubsystem",
				"AssetRegistry",
				"DeveloperSettings",
				"Blutility",
				"UMG",
				"EngineSettings",
				"WebBrowser",
				"Projects",
                "VertexPaintDetectionPlugin",
                "GameplayTags",
                "GameplayTagsEditor",
				"RapidJsonModule"
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
