// Copyright 2023 Alexander Floden, Alias Alex River. All Rights Reserved. 

using UnrealBuildTool;
using System.IO;

public class RapidJsonModule : ModuleRules
{
    public RapidJsonModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json"});

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Add the public include paths
        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "ThirdParty/rapidjson")
            }
        );
    }
}
