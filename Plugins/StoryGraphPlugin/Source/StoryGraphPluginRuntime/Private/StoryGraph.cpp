// Copyright 2016 Dmitriy Pavlov

#include "StoryGraph.h"
#include "CustomNodes.h"
#include "EdGraph/EdGraph.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "SaveGameInstance.h"
#include "StoryGraphObject.h"
#include "StoryGraphWidget.h"
#include "StorySceneObject.h"

//UStoryGraphBlueprint...............................................

UEdGraph_StoryGraph* UStoryGraphBlueprint::FindGraph(UObject* GraphOwner)
{
	for (int i = 0; i < Graphs.Num(); i++)
	{
		if (Graphs[i]->GraphOwner == GraphOwner)
		{
			return Graphs[i];
		}
	}

	return nullptr;
}


//UExecutionTree...................................................................
void UExecutionTree::Refresh()
{
	TArray<UCustomNodeBase*> ChildNodes;

	TArray<UStoryVerticalNodeBase*> NodsForAdd;

	if (MainQuest->GetCurrentState() == (int)EQuestStates::Active ||
		MainQuest->GetCurrentState() == (int)EQuestStates::UnActive)
	{
		bool bAllNodesPerformed = false;

		while (!bAllNodesPerformed)
		{
			bAllNodesPerformed = true;

			for (int j = 0; j < ActiveNodesBuffer.Num(); j++)
			{
				if (UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(ActiveNodesBuffer[j]))
				{
					EPerformNodeResult PerformResult = VerticalNode->PerformNode();

					if (PerformResult == EPerformNodeResult::Successed)
					{
						VerticalNode->ResetUnPerformBrunch();
						//if we have already perform node, we must reset others ways for this node
						VerticalNode->GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);
						if (ChildNodes.Num() == 0)
						{
							MainQuest->SetCurrentState((int)EQuestStates::Complete);
							//We find end of tree an quest complete
							break;
						}
						for (int i = 0; i < ChildNodes.Num(); i++)
						{
							NodsForAdd.Add((UStoryVerticalNodeBase*)ChildNodes[i]);
						}
						bAllNodesPerformed = false;
					}
					else if (PerformResult == EPerformNodeResult::Fail)
					{
						NodsForAdd.Add(ActiveNodesBuffer[j]);
					}
				}
			}

			ActiveNodesBuffer.Empty();
			ActiveNodesBuffer.Append(NodsForAdd);
			NodsForAdd.Empty();
		}
	}
}

//UStoryGraph...........................................................................

void UStoryGraph::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	if (CompilationCounter != LoadedCompilationCounter)
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Use old save file"));
		//If Compilercounters mismatch this mean we use old save file
		OldSaveFile = true;
	}

	Objects.Append(GraphObjects);

	for (int i = 0; i < GraphNodes.Num(); i++)
	{
		if (Cast<UStoryVerticalNodeBase>(GraphNodes[i]))
		{
			Objects.Add(GraphNodes[i]);
		}
	}

	if (WantedObjectsNum == 0) //Save object
	{
		Objects.Append(ExecutionTrees);
	}
	else //Load object
	{
		int NodeNum = 0;
		for (int i = 0; i < GraphNodes.Num(); i++)
		{
			if (Cast<UStoryVerticalNodeBase>(GraphNodes[i]))
			{
				NodeNum++;
			}
		}
		int ExecutionTreeNum = WantedObjectsNum - GraphObjects.Num() - NodeNum;
		for (int i = 0; i < ExecutionTreeNum; i++)
		{
			UExecutionTree* NewExecutionTree = NewObject<UExecutionTree>(this);
			ExecutionTrees.Add(NewExecutionTree);
			Objects.Add(NewExecutionTree);
		}
	}
}


void UStoryGraph::CreateExecutionTrees()
{
	TArray<UQuestStartNode*> QuestStartNodes;
	ExecutionTrees.Empty();

	for (int i = 0; i < GraphNodes.Num(); i++)
	{
		if (UQuestStartNode* QuestStartNode = Cast<UQuestStartNode>(GraphNodes[i]))
		{
			QuestStartNodes.Add(QuestStartNode);
		}
	}

	for (int i = 0; i < QuestStartNodes.Num(); i++)
	{
		UExecutionTree* NewExecutionTree = NewObject<UExecutionTree>(this);
		ExecutionTrees.Add(NewExecutionTree);
		NewExecutionTree->MainQuest = (UStoryGraphQuest*)QuestStartNodes[i]->pGraphObject;
		NewExecutionTree->ActiveNodesBuffer.Add(QuestStartNodes[i]);
	}
}

void UStoryGraph::RefreshExecutionTrees(bool NeedRefreshQuestsPhase)
{
	if (OldSaveFile)
	{
		GWorld->GetFirstPlayerController()->ConsoleCommand("Exit");
		return;
	}
	for (int i = 0; i < ExecutionTrees.Num(); i++)
	{
		ExecutionTrees[i]->Refresh();
	}

	if (QuestStateWasChange)
	{
		QuestStateWasChange = false;
		RefreshExecutionTrees(false);
	}

	if (NeedRefreshQuestsPhase)
	{
		RefreshQuestsPhase();
		RefreshRadarTargets();
	}
}

void UStoryGraph::RefreshQuestsPhase()
{
	for (int i = 0; i < GraphObjects.Num(); i++)
	{
		UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(GraphObjects[i]);
		if (Quest && Quest->GetCurrentState() == (int)EQuestStates::Active)
		{
			for (int j = 0; j < Quest->QuestPhase.Num(); j++)
			{
				Quest->QuestPhase[j]->QuestPhaseState = EQuestPhaseState::UnActive;
			}
		}
	}

	for (int i = 0; i < ExecutionTrees.Num(); i++)
	{
		for (int j = 0; j < ExecutionTrees[i]->ActiveNodesBuffer.Num(); j++)
		{
			if (UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(
				ExecutionTrees[i]->ActiveNodesBuffer[j]))
			{
				if (VerticalNode->pQuestPhase)
				{
					VerticalNode->pQuestPhase->QuestPhaseState = EQuestPhaseState::Active;
				}
			}
		}
	}
}

void UStoryGraph::RefreshRadarTargets()
{
	TArray<class UQuestPhase*> RadarTargets;

	for (int i = 0; i < GraphObjects.Num(); i++)
	{
		UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(GraphObjects[i]);
		if (Quest && Quest->GetCurrentState() == (int)EQuestStates::Active)
		{
			for (int j = 0; j < Quest->QuestPhase.Num(); j++)
			{
				if (Quest->QuestPhase[j]->QuestPhaseState == EQuestPhaseState::Active)
				{
					RadarTargets.Add(Quest->QuestPhase[j]);
				}
			}
		}
	}
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		if (HUD->Radar)
		{
			HUD->Radar->RefreshTargets(RadarTargets);
		}
	}
}

template <class MinRequiredType>
void UStoryGraph::GetGraphObjectsOfClass(TArray<MinRequiredType*>& OutObjects) const
{
	for (int32 i = 0; i < GraphObjects.Num(); i++)
	{
		UStoryGraphObject* GraphObject = GraphObjects[i];
		if (MinRequiredType* TypedGraphObject = Cast<MinRequiredType>(GraphObject))
		{
			OutObjects.Add(TypedGraphObject);
		}
	}
}

//AStoryGraphActor.....................................................................................

AStoryGraphActor::AStoryGraphActor()
{
	StoryGraph = nullptr;
}


bool AStoryGraphActor::CreateStoryGraph()
{
	if ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)
	{
		if (!StoryGraph || StoryGraph->CompilationCounter != ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)
		                                                     ->StoryGraph->CompilationCounter)
		{
			StoryGraph = DuplicateObject<UStoryGraph>(((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)->StoryGraph,
			                                          this);
			if (StoryGraph)
			{
				for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
				{
					UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
						StoryGraph->GraphObjects[i]);
					if (ObjectWithSceneObject)
					{
						ObjectWithSceneObject->SetSceneObjectRealPointers();
					}
				}

				CompilationCounter = StoryGraph->CompilationCounter;
				return true;
			}
		}
	}

	return false;
}

void AStoryGraphActor::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	if (StoryGraph)
	{
		Objects.Add(StoryGraph);
	}
}

void AStoryGraphActor::ClearCrossLevelReferences()
{
	Super::ClearCrossLevelReferences();
	CreateStoryGraph();
}

void AStoryGraphActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();


	if (StoryGraph)
	{
		StoryGraph->OwnedActor = this;

		for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
		{
			if (UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
				StoryGraph->GraphObjects[i]))
			{
				TArray<IStorySceneObject*> SceneObjects;
				ObjectWithSceneObject->GetSceneObjects(SceneObjects);

				for (int j = 0; j < SceneObjects.Num(); j++)
				{
					SceneObjects[j]->OwningStoryGraphObject.Add(
						(UStoryGraphObjectWithSceneObject*)StoryGraph->GraphObjects[i]);
				}
			}
		}

		USaveGameInstance* SaveGameInstance = Cast<USaveGameInstance>(GetGameInstance());

		if (!(SaveGameInstance && SaveGameInstance->IsLevelLoaded))
		{
			StoryGraph->CreateExecutionTrees();
		}
	}
}


void AStoryGraphActor::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
	{
		if (UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
			StoryGraph->GraphObjects[i]))
		{
			ObjectWithSceneObject->InitializeObject(); //Set initial active state
		}
	}

	StoryGraph->RefreshExecutionTrees();
}

void AStoryGraphActor::Serialize(FArchive& Ar)
{
#if WITH_EDITOR
	if (Ar.IsSaving() && (Ar.GetPortFlags() & PPF_DuplicateForPIE))
	{
		if (CreateStoryGraph())
		{
			MarkPackageDirtyCustom();
		}
	}
	/*if (Ar.IsLoading() && (Ar.GetPortFlags() & PPF_DuplicateForPIE))
	{
		
		
	}*/
#endif

	Super::Serialize(Ar);
}

void AStoryGraphActor::MarkPackageDirtyCustom() const
{
	UPackage* Package = GetOutermost();

	if (Package != nullptr)
	{
		const bool bIsDirty = Package->IsDirty();

		if (!bIsDirty)
		{
			Package->SetDirtyFlag(true);
		}

		Package->PackageMarkedDirtyEvent.Broadcast(Package, bIsDirty);
	}
}
