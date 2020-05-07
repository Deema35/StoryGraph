#pragma once
#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "StoryGraphBlueprint.generated.h"

class UEdGraph_StoryGraph;
UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UStoryGraphBlueprint : public UBlueprint
{
    GENERATED_BODY()
public:
    UPROPERTY()
    class UStoryGraph* StoryGraph;

    UPROPERTY()
    TArray<UEdGraph_StoryGraph*> Graphs;
    public:
    UEdGraph_StoryGraph* FindGraph(UObject* GraphOwner);

    void AddGraph(UEdGraph_StoryGraph* Graph) { Graphs.Add(Graph); }

    void RemoveGraph(UEdGraph_StoryGraph* Graph) { Graphs.RemoveSingle(Graph); }
};
