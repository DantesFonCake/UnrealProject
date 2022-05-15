// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyProject : ModuleRules
{
	public MyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{   "Core", 
			"CoreUObject", 
			"Engine",
			"RenderCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "CustomFluidShader" });

		PrivateIncludePathModuleNames.AddRange(new string[] { "CustomFluidShader" });
	}
}
