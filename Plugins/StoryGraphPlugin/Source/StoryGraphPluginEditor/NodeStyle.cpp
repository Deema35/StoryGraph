// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "NodeStyle.h"
#include "SlateGameResources.h"

TSharedPtr< FSlateStyleSet > FNodeStyle::NodeStyleInstance = NULL;

void FNodeStyle::Initialize()
{
	if ( !NodeStyleInstance.IsValid() )
	{
		NodeStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle( *NodeStyleInstance );
	}
}

void FNodeStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *NodeStyleInstance );
	ensure( NodeStyleInstance.IsUnique() );
	NodeStyleInstance.Reset();
}

FName FNodeStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CustomNodeStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::ProjectPluginsDir() / "StoryGraphPlugin/Resources"/ RelativePath + TEXT(".png"), __VA_ARGS__ )


TSharedRef< FSlateStyleSet > FNodeStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet("CustomNodeStyle"));
	FSlateStyleSet& Style = StyleRef.Get();


	// The image used to draw the replay pause button
	Style.Set("CustomNode.Radar", new IMAGE_BRUSH("NodeIcons/Radar_64x", FVector2D(64.0f, 64.0f)));
	Style.Set("CustomNode.QuestStart", new IMAGE_BRUSH("NodeIcons/QuestStartIcon_x64", FVector2D(64.0f, 64.0f)));
	

	return StyleRef;
}

#undef IMAGE_BRUSH


void FNodeStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FNodeStyle::Get()
{
	return *NodeStyleInstance;
}
