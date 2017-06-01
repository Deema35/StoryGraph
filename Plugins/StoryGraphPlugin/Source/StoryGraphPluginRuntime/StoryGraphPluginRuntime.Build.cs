// Copyright 2016 Dmitriy Pavlov

using UnrealBuildTool;

public class StoryGraphPluginRuntime : ModuleRules
{
	public StoryGraphPluginRuntime(ReadOnlyTargetRules ROTargetRules) : base(ROTargetRules)
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
