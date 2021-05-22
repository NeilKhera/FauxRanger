using UnrealBuildTool;

using System;
using System.IO;
using System.Text.RegularExpressions;

public class FauxRanger : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	public FauxRanger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ROSIntegration" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.Add(Path.Combine(ModulePath, "../mINI/src"));

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
