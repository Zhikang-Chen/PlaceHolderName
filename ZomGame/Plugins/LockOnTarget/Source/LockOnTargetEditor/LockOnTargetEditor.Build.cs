// Copyright 2022-2023 Ivan Baktenkov. All Rights Reserved.

using UnrealBuildTool;

public class LockOnTargetEditor : ModuleRules
{
    public LockOnTargetEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "LockOnTarget",
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "InputCore",
                "PropertyEditor",
                "UnrealEd",
                "SceneOutliner", //SSocketChooserPopup

            }
            );
    }
}
