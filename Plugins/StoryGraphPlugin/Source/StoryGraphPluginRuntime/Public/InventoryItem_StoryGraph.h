#pragma once
#include "SceneObjectActor_StoryGraph.h"
#include "InventoryItem_StoryGraph.generated.h"

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API AInventoryItem_StoryGraph : public ASceneObjectActor_StoryGraph
{
    GENERATED_BODY()
public:


    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    void PickUp();

    virtual void RefreshSceneObjectsActive() override;
};