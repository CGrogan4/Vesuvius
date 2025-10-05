// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VesuviusOne : ModuleRules
{
	public VesuviusOne(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Add additional public dependencies: Niagara, UMG, PhysicsCore, Slate, SlateCore
        PublicDependencyModuleNames.AddRange(new string[] {"Core","CoreUObject","Engine","InputCore","Niagara","UMG","PhysicsCore","Slate","SlateCore"});

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
