#pragma once
#include "GameFramework/Actor.h"
#include "StorySceneObject.h"
#include "SaveObject_StoryGraph.h"
#include "SceneObjectActor_StoryGraph.generated.h"


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API ASceneObjectActor_StoryGraph : public AActor, public IStorySceneObject,
                                                                 public ISaveObject_StoryGraph
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
    void GetMessageFromStoryGraph(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    FText GetObjectName();

    virtual void SendMessageToSceneObject(FString Message) override;

    virtual void EnableObjectOnMap(bool IsEnabledOnMap) override;

    void SetStoryGraphObjectState(int NewState) override;
};
