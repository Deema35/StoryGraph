#pragma once
#include "SceneObjectActor_StoryGraph.h"
#include "Engine/LevelScriptActor.h"
#include "OtherActor_StoryGraph.generated.h"


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API AOtherActor_StoryGraph : public ASceneObjectActor_StoryGraph
{
    GENERATED_BODY()
public:
};

UCLASS()
class ALevelScriptActor_StoryGraph : public ALevelScriptActor
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
    void GetMessageFromStoryGraph(const FString& Message);
};