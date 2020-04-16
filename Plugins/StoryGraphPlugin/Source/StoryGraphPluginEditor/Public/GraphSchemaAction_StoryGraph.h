// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "EdGraph/EdGraphSchema.h"
#include "StoryGraphObject.h"

struct FEdGraphSchemaAction_StoryGraph : public FEdGraphSchemaAction
{
	FEdGraphSchemaAction_StoryGraph(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping, const int32 InSectionID)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, FText(), InSectionID)
	{}
	
	int32 StoryGraphObjectNum;
};
