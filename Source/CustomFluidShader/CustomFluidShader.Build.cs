// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CustomFluidShader : ModuleRules
{
	public CustomFluidShader(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PrivateDependencyModuleNames.AddRange(new string[]
		{
				"Core",
				"CoreUObject",
				"Engine",
				"Renderer",
				"RenderCore",
				"RHI",
				"Projects"
		});
	}
}
