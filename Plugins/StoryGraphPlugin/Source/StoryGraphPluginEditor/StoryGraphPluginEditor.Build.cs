// Copyright 2016 Dmitriy Pavlov

using UnrealBuildTool;

public class StoryGraphPluginEditor : ModuleRules
{
	public StoryGraphPluginEditor(TargetInfo Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; // Enabel IWYU

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "InputCore",
            "Engine",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "PropertyEditor",
            "UnrealEd",
            "EditorStyle",
            "GraphEditor",
            "BlueprintGraph",
            "Kismet",
            "XmlParser",
            "DesktopPlatform",
        });



        PrivateDependencyModuleNames.AddRange(new string[] { "StoryGraphPluginRuntime" }); //Основной модуль
		
		
	}
}
