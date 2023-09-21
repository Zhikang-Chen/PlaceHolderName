// Copyright 2022 ZADROT LLC

using UnrealBuildTool;
class EIS : ModuleRules
{
	public EIS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{ "Core", "CoreUObject", "Slate", "Engine" }
		);
	}
}