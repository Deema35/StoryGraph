// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class StoryGraphTarget : TargetRules
{
	public StoryGraphTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "StoryGraph" });
    }

	//
	// TargetRules interface.
	//

	
}
