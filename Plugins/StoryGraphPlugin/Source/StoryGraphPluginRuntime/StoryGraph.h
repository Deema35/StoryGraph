// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "Object.h"
#include "Classes/Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "StoryGraph.generated.h"

class UCustomNodeBase;
class FAssetEditor_StoryGraph;



UENUM()
enum class EStoryGraphState : uint8
{
	ST_Modify,
	ST_Compile,
	ST_Error
};



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
};


UCLASS(NotBlueprintable)
class STORYGRAPHPLUGINRUNTIME_API AStoryGraphActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UStoryGraph* StoryGraph;

	TArray<AActor*> LinkStorage;

	UPROPERTY(VisibleAnywhere)
		int32 CompilationCounter;
	

public:
	AStoryGraphActor();

	virtual void PreInitializeComponents() override;

	//virtual void PostLoad() override;


	virtual void BeginPlay() override;

	virtual void Serialize(FArchive& Ar) override;

	virtual void ClearCrossLevelReferences() override;

	bool CreateStoryGraph();

private:

	void MarkPackageDirtyCustom() const;
};


struct FExecutionTree
{
public:
	class UQuestStartNode* QuestStartNode;
	
	TArray<UStoryVerticalNodeBase*> PredActiveNodesBuffer;


public:
	void Refresh();

};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UStoryGraph : public UObject
{
	GENERATED_BODY()

	
public:
	UPROPERTY()
	int32 CompilationCounter;

	UPROPERTY()
	TArray<class UStoryGraphObject*> GarphObjects;

	UPROPERTY()
		TArray<class UCustomNodeBase*> GarphNods;

	TArray<FExecutionTree> ExecutionTrees;

	class UGameScreen_StoryGraphWidget* GameScreen;

	class URadar_StoryGraphWidget* Radar;

	bool QuestStateWasChange; // If quest state was change during refresh execution tree, tree must be refresh again.
	
#if WITH_EDITORONLY_DATA
	FAssetEditor_StoryGraph* pAssetEditor;

	UPROPERTY()
		EStoryGraphState StoryGraphState;
#endif //WITH_EDITORONLY_DATA
	
public:

	void CreateExecutionTrees();

	void RefreshExecutionTrees(bool NeedRefreshQuestsPhase = true);

	void RefreshQuestsPhase();

	void RefreshRadarTargets();
//.....................................................................................................
private:
	/** Gets all the Graph Objects of a given type */
	template<class MinRequiredType>
	inline void GetGraphObjectsOfClass(TArray<MinRequiredType*>& OutObjects) const
	{
		for (int32 i = 0; i < GarphObjects.Num(); i++)
		{
			UStoryGraphObject* GarphObject = GarphObjects[i];
			if (MinRequiredType* TypedGarphObject = Cast<MinRequiredType>(GarphObject))
			{
				OutObjects.Add(TypedGarphObject);
			}
		}
	}

};

