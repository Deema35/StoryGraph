// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "CoreMinimal.h"
#include "SaveObject_StoryGraph.h"

#include "StoryGraph.generated.h"


UENUM()
enum class EStoryGraphState : uint8
{
	ST_Modify,
	ST_Compile,
	ST_Error
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
	TArray<class UStoryGraphObject*> GraphObjects;

	UPROPERTY()
	TArray<class UCustomNodeBase*> GraphNodes;

	UPROPERTY()
	TArray<class UExecutionTree*> ExecutionTrees;


	bool QuestStateWasChange; // If quest state was change during refresh execution tree, tree must be refresh again.


#if WITH_EDITORONLY_DATA
	class FAssetEditor_StoryGraph* pAssetEditor;

	UPROPERTY()
	EStoryGraphState StoryGraphState;
#endif //WITH_EDITORONLY_DATA

	UPROPERTY()
	AActor* OwnedActor;

private:

	bool OldSaveFile;


public:

	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;
	// ISaveObject_StoryGraph interface

	void CreateExecutionTrees();

	void RefreshExecutionTrees(bool NeedRefreshQuestsPhase = true);

	void RefreshQuestsPhase();

	void RefreshRadarTargets();

private:
	/** Gets all the Graph Objects of a given type */
	template <class MinRequiredType>
	void GetGraphObjectsOfClass(TArray<MinRequiredType*>& OutObjects) const;
};
