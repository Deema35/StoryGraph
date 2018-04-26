// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "Object.h"
#include "GameFramework/Actor.h"
#include "StoryGraphObject.h"
#include "ObjectrRecord.h"
#include "Engine/LevelScriptActor.h"
#include "StoryScenObject.generated.h"

class STORYGRAPHPLUGINRUNTIME_API IStoryScenObject
{
public:
	virtual ~IStoryScenObject() {}
	TArray<UStoryGraphObjectWithScenObject*> OwningStoryGraphObject;

	bool IsEnabel;


public:
	IStoryScenObject() : IsEnabel(true) {}
	virtual void RefreshScenObjectActive();
	virtual void EnabelObjectOnMap(bool Enabel) = 0;
	virtual void SetStoryGraphObjectState(int NewState) = 0;
	virtual void SendMessageToScenObject(FString Message) = 0;

};


UCLASS()

class STORYGRAPHPLUGINRUNTIME_API ACharecter_StoryGraph : public ACharacter, public IStoryScenObject, public ISaveObject_StoryGraph
{
	

	GENERATED_BODY()
public:

	ACharecter_StoryGraph();

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void ChangeState(ECharecterStates NewState);

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		void OpenDialog();

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		FText GetObjectName();
	
	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void GetMessegeFromStoryGraph(const FString& Message);

	virtual void EnabelObjectOnMap(bool Enabel) override;

	virtual void SendMessageToScenObject(FString Message) override;

	virtual void SetStoryGraphObjectState(int NewState) override;
};


UCLASS()

class STORYGRAPHPLUGINRUNTIME_API AScenObjectActor_StoryGraph : public AActor ,public IStoryScenObject , public ISaveObject_StoryGraph
{
	GENERATED_BODY()

public:
	

	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
	void GetMessegeFromStoryGraph(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		FText GetObjectName();

	virtual void SendMessageToScenObject(FString Message) override;

	virtual void EnabelObjectOnMap(bool Enabel) override;

	virtual void SetStoryGraphObjectState(int NewState) override;
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API APlaceTrigger_StoryGraph : public AScenObjectActor_StoryGraph
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

class STORYGRAPHPLUGINRUNTIME_API AInventoryItem_StoryGraph : public AScenObjectActor_StoryGraph
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void PickUp();

	virtual void RefreshScenObjectActive() override;

};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API AOtherActor_StoryGraph : public AScenObjectActor_StoryGraph
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
		void GetMessegeFromStoryGraph(const FString& Message);

};