#pragma once
#include "SceneObjectActor_StoryGraph.h"
#include "PlaceTrigger_StoryGraph.generated.h"

enum class EPlaceTriggerStates : unsigned char;

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API APlaceTrigger_StoryGraph : public ASceneObjectActor_StoryGraph
{
    GENERATED_BODY()
public:


    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    EPlaceTriggerType GetPlaceTriggerType();


    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    void ChangeState(EPlaceTriggerStates NewState);

    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    void Activate();
};
