#pragma once
#include "CoreMinimal.h"
#include "ExecutionTree.generated.h"

UCLASS()
class UExecutionTree : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame)
    class UStoryGraphQuest* MainQuest;

    UPROPERTY(SaveGame)
    TArray<class UStoryVerticalNodeBase*> ActiveNodesBuffer;

public:
    void Refresh();
};