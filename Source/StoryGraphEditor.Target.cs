// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class StoryGraphEditorTarget : TargetRules
{
	public StoryGraphEditorTarget(TargetInfo Target) : base(Target)
    {
	    DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Editor;
        ExtraModuleNames.AddRange(new string[] { "StoryGraph" });
    }

	//
	// TargetRules interface.
	//

	
}
