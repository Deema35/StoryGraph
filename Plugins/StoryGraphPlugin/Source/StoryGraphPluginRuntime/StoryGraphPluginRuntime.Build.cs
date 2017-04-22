// Copyright 2016 Dmitriy Pavlov

using UnrealBuildTool;

public class StoryGraphPluginRuntime : ModuleRules
{
	public StoryGraphPluginRuntime(TargetInfo Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; // Enabel IWYU

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
             "UMG",
            "Slate",
            "SlateCore"
        });
       

    }
}
