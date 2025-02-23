// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Eclipse_Syndrome : ModuleRules
{
	public Eclipse_Syndrome(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	

		PublicDependencyModuleNames.AddRange(new string[] {
			"CableComponent",
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
			"UMG",
            "Niagara",
            "ProceduralMeshComponent"
		});

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
