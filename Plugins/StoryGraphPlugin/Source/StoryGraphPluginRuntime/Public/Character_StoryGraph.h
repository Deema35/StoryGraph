#pragma once
#include "StorySceneObject.h"
#include "GameFramework/Character.h"
#include "Character_StoryGraph.generated.h"

UENUM(BlueprintType)
enum class ECharacterStates : uint8
{
    Alive,
    Dead
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API ACharacter_StoryGraph : public ACharacter, public IStorySceneObject,
                                                          public ISaveObject_StoryGraph
{
    GENERATED_BODY()
public:

    ACharacter_StoryGraph();

    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    void ChangeState(ECharacterStates NewState);

    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    void OpenDialog();

    UFUNCTION(BlueprintCallable, Category = StoryGraph)
    FText GetObjectName();

    UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
    void GetMessageFromStoryGraph(const FString& Message);

    virtual void EnableObjectOnMap(bool IsEnabledOnMap) override;

    virtual void SendMessageToSceneObject(FString Message) override;

    virtual void SetStoryGraphObjectState(int NewState) override;
};
