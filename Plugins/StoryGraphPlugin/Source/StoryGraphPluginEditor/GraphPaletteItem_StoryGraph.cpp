// Copyright 2016 Dmitriy Pavlov
#include "StoryGraphPluginEditor.h"
#include "GraphPaletteItem_StoryGraph.h"

void SGraphPaletteItem_StoryGraph::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	OnNameTextCommittedDelegate.ExecuteIfBound(NewText, InTextCommit);
}