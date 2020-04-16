// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "SlateBasics.h"
#include "Commands.h"
#include "EditorStyle.h"

class FCommands_StoryGraph : public TCommands<FCommands_StoryGraph>
{
public:

	FCommands_StoryGraph() : TCommands<FCommands_StoryGraph>(TEXT("Story Graph Commands"), FText::FromString("Story Graph Commands"), NAME_None, FEditorStyle::GetStyleSetName())
	{}

	virtual void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> CheckObjects;
	TSharedPtr<FUICommandInfo> SaveAsset;
	TSharedPtr<FUICommandInfo> ExportInXML;
	TSharedPtr<FUICommandInfo> ImportFromXML;
	TSharedPtr<FUICommandInfo> FindInContentBrowser;
	TSharedPtr<FUICommandInfo> UnlinkAllObjects;
};