#include "StoryGraphBlueprint.h"
#include "Graph_StoryGraph.h"

UEdGraph_StoryGraph* UStoryGraphBlueprint::FindGraph(UObject* GraphOwner)
{
	for (int i = 0; i < Graphs.Num(); i++)
	{
		if (Graphs[i]->GraphOwner == GraphOwner)
		{
			return Graphs[i];
		}
	}

	return nullptr;
}