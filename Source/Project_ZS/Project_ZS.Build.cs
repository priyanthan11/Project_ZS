// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_ZS : ModuleRules
{
	public Project_ZS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ProceduralMeshComponent" });
	}
}
