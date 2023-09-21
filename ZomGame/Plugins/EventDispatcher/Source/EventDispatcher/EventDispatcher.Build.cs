// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EventDispatcher : ModuleRules
{
	public EventDispatcher(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Private/EventDispatcherPrivatePCH.h";

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
			
		if (Target.Version.MinorVersion >= 26 || Target.Version.MajorVersion == 5)
        {
            PrivateDependencyModuleNames.Add("DeveloperSettings");
        }
	}
}
