// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "Object.h"
#include "Classes/Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "ObjectrRecord.h"
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

	void AddGraph(UEdGraph_StoryGraph* Graph) { Graphs.Add(Graph); }

	void RemoveGraph(UEdGraph_StoryGraph* Graph) { Graphs.RemoveSingle(Graph); }
};


UCLASS(NotBlueprintable)
class STORYGRAPHPLUGINRUNTIME_API AStoryGraphActor : public AActor, public ISaveObject_StoryGraph
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


	virtual void BeginPlay() override;

	virtual void Serialize(FArchive& Ar) override;

	virtual void ClearCrossLevelReferences() override;

	bool CreateStoryGraph();
	
	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override; // ISaveObject_StoryGraph interface

private:

	void MarkPackageDirtyCustom() const;
};

UCLASS()
class UExecutionTree : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(SaveGame)
	class UStoryGraphQuest* MainQuest;

	UPROPERTY(SaveGame)
	TArray<class UStoryVerticalNodeBase*> PredActiveNodesBuffer;

public:
	void Refresh();

};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UStoryGraph : public UObject, public ISaveObject_StoryGraph
{
	GENERATED_BODY()

	
public:
	UPROPERTY()
	int32 CompilationCounter;

	UPROPERTY(SaveGame)
		int32 LoadedCompilationCounter;

	UPROPERTY()
	TArray<class UStoryGraphObject*> GarphObjects;

	UPROPERTY()
		TArray<class UCustomNodeBase*> GarphNods;

	UPROPERTY()
	TArray<UExecutionTree*> ExecutionTrees;


	bool QuestStateWasChange; // If quest state was change during refresh execution tree, tree must be refresh again.

	
#if WITH_EDITORONLY_DATA
	FAssetEditor_StoryGraph* pAssetEditor;

	UPROPERTY()
		EStoryGraphState StoryGraphState;
#endif //WITH_EDITORONLY_DATA

	AActor* OwnedActor = nullptr;

private:

	bool OldSaveFile;

	
	
public:
	
	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;// ISaveObject_StoryGraph interface

	void CreateExecutionTrees();

	void RefreshExecutionTrees(bool NeedRefreshQuestsPhase = true);

	void RefreshQuestsPhase();

	void RefreshRadarTargets();

private:
	/** Gets all the Graph Objects of a given type */
	template<class MinRequiredType>
	void GetGraphObjectsOfClass(TArray<MinRequiredType*>& OutObjects) const;
	

};

