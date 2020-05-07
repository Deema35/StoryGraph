#pragma once
#include "SaveObject_StoryGraph.h"
#include "GameFramework/Actor.h"
#include "StoryGraphActor.generated.h"

UCLASS(NotBlueprintable)
class STORYGRAPHPLUGINRUNTIME_API AStoryGraphActor : public AActor, public ISaveObject_StoryGraph
{
    GENERATED_BODY()

public:
    UPROPERTY()
    class UStoryGraph* StoryGraph;

    UPROPERTY()
    TArray<AActor*> LinkStorage;

    UPROPERTY(VisibleAnywhere)
    int32 CompilationCounter;


    public:
    AStoryGraphActor();

    virtual void PreInitializeComponents() override;


    virtual void BeginPlay() override;

    virtual void Serialize(FArchive& Ar) override;

    virtual void ClearCrossLevelReferences() override;

    bool CreateStoryGraph();

    virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;
    // ISaveObject_StoryGraph interface

    private:

        void MarkPackageDirtyCustom() const;
};