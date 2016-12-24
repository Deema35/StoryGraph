// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "EdGraph/EdGraph.h"
#include "Graph_StoryGraph.generated.h"

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UEdGraph_StoryGraph : public UEdGraph
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	class UStoryGraph* StoryGraph;

	UPROPERTY()
		UObject* GraphOwner;

public:

	class UStoryGraph* GetStoryGraph() const { return StoryGraph; }
};