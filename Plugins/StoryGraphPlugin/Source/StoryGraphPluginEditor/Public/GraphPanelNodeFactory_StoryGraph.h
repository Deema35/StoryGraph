// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "EdGraphUtilities.h"

class  FGraphPanelNodeFactory_StoryGraph : public FGraphPanelNodeFactory
{
public:
	FGraphPanelNodeFactory_StoryGraph();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
