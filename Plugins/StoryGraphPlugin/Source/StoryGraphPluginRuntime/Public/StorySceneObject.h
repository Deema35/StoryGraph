// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "Object.h"
#include "GameFramework/Actor.h"
#include "StoryGraphObject.h"
#include "ObjectRecord.h"
#include "Engine/LevelScriptActor.h"
#include "StorySceneObject.generated.h"

class STORYGRAPHPLUGINRUNTIME_API IStorySceneObject
{
public:
	virtual ~IStorySceneObject()
	{
	}

	TArray<UStoryGraphObjectWithSceneObject*> OwningStoryGraphObject;

	bool IsEnabled;


public:
	IStorySceneObject() : IsEnabled(true)
	{
	}

	virtual void RefreshSceneObjectsActive();
	virtual void EnableObjectOnMap(bool IsEnabled) = 0;
	virtual void SetStoryGraphObjectState(int NewState) = 0;
	virtual void SendMessageToSceneObject(FString Message) = 0;
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

	virtual void EnableObjectOnMap(bool IsEnabled) override;

	virtual void SendMessageToSceneObject(FString Message) override;

	virtual void SetStoryGraphObjectState(int NewState) override;
};


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

	virtual void EnableObjectOnMap(bool IsEnabled) override;

	void SetStoryGraphObjectState(int NewState) override;
};

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

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API AInventoryItem_StoryGraph : public ASceneObjectActor_StoryGraph
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void PickUp();

	virtual void RefreshSceneObjectsActive() override;
};

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
