// Copyright 2016 Dmitriy Pavlov

#include "StoryGraph.h"
#include "CustomNods.h"
#include "EdGraph/EdGraph.h"
#include "StoryGraphObject.h"
#include "StoryScenObject.h"
#include "HUD_StoryGraph.h"
#include "StoryGraphWiget.h"
#include "SaveGameInstance.h"
#include "LogCategoryRutime.h"

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

	return NULL;
}




//UExecutionTree...................................................................
void UExecutionTree::Refresh()
{
	TArray<UCustomNodeBase*> ChildNodes;

	TArray<UStoryVerticalNodeBase*> NodsForAdd;
	bool AllNodsPerfomed;

	if (MainQuest->GetCurentState() == (int)EQuestStates::Active ||
		MainQuest->GetCurentState() == (int)EQuestStates::UnActive)
	{

		AllNodsPerfomed = false;

		while (!AllNodsPerfomed)
		{

			AllNodsPerfomed = true;

			for (int j = 0; j < PredActiveNodesBuffer.Num(); j++)
			{
				if (UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(PredActiveNodesBuffer[j]))
				{
					EPerformNodeResult PerformResult = VerticalNode->PerformNode();

					if (PerformResult == EPerformNodeResult::Sucssed)
					{

						VerticalNode->ResetUnPerformBrunch(); //if we have already perform node, we must reset others ways for this node
						VerticalNode->GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);
						if (ChildNodes.Num() == 0)
						{

							MainQuest->SetCurentState((int)EQuestStates::Complite); //We find end of tree an quest complite
							break;
						}
						for (int i = 0; i < ChildNodes.Num(); i++)
						{
							NodsForAdd.Add((UStoryVerticalNodeBase*)ChildNodes[i]);
						}
						AllNodsPerfomed = false;


					}
					else if (PerformResult == EPerformNodeResult::Fail)
					{
						NodsForAdd.Add(PredActiveNodesBuffer[j]);

					}
				}
				

			}

			PredActiveNodesBuffer.Empty();
			PredActiveNodesBuffer.Append(NodsForAdd);
			NodsForAdd.Empty();

		}


	}
	
	
}

//UStoryGraph...........................................................................

void UStoryGraph::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	if (CompilationCounter != LoadedCompilationCounter)
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Use old save file"));//If Compilercounters mismatch this mean we use old save file
		OldSaveFile = true;
	}

	Objects.Append(GarphObjects);

	for (int i = 0; i < GarphNods.Num(); i++)
	{
		if (Cast<UStoryVerticalNodeBase>(GarphNods[i]))
		{
			Objects.Add(GarphNods[i]);
		}

	}

	if (WantedObjectsNum == 0) //Save object
	{
		Objects.Append(ExecutionTrees);
	}
	else //Load object
	{
		int NodeNum = 0;
		for (int i = 0; i < GarphNods.Num(); i++)
		{
			if (Cast<UStoryVerticalNodeBase>(GarphNods[i]))
			{
				NodeNum++;
			}
		}
		int ExecutionTreeNum = WantedObjectsNum - GarphObjects.Num() - NodeNum;
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
	
	for (int i = 0; i < GarphNods.Num(); i++)
	{
		if (UQuestStartNode* QuestStartNode = Cast<UQuestStartNode>(GarphNods[i]))
		{
			
			QuestStartNodes.Add(QuestStartNode);
		}
	}
	
	for (int i = 0; i < QuestStartNodes.Num(); i++)
	{
		UExecutionTree* NewExecutionTree = NewObject<UExecutionTree>(this);
		ExecutionTrees.Add(NewExecutionTree);
		NewExecutionTree->MainQuest = (UStoryGraphQuest*)QuestStartNodes[i]->pGraphObject;
		NewExecutionTree->PredActiveNodesBuffer.Add(QuestStartNodes[i]);
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
	for (int i = 0; i < GarphObjects.Num(); i++)
	{
		UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(GarphObjects[i]);
		if (Quest && Quest->GetCurentState() == (int)EQuestStates::Active)
		{
			for (int j = 0; j < Quest->QuestPhase.Num(); j++)
			{
				Quest->QuestPhase[j]->QuestPhaseState = EQuestPhaseState::UnActive;
			}
		}
	}

	for (int i = 0; i < ExecutionTrees.Num(); i++)
	{
		for (int j = 0; j <  ExecutionTrees[i]->PredActiveNodesBuffer.Num(); j++)
		{
			if (UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(ExecutionTrees[i]->PredActiveNodesBuffer[j]))
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

	for (int i = 0; i < GarphObjects.Num(); i++)
	{
		UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(GarphObjects[i]);
		if (Quest && Quest->GetCurentState() == (int)EQuestStates::Active)
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

template<class MinRequiredType>
void UStoryGraph::GetGraphObjectsOfClass(TArray<MinRequiredType*>& OutObjects) const
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

//AStoryGraphActor.....................................................................................

AStoryGraphActor::AStoryGraphActor()
{
	
	StoryGraph = NULL;
}



bool AStoryGraphActor::CreateStoryGraph()
{
	if ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)
	{
		if (!StoryGraph || StoryGraph->CompilationCounter != ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)->StoryGraph->CompilationCounter)
		{
			StoryGraph = DuplicateObject<UStoryGraph>(((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)->StoryGraph, this);
			if (StoryGraph)
			{
				
				for (int i = 0; i < StoryGraph->GarphObjects.Num(); i++)
				{
					UStoryGraphObjectWithScenObject* ObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(StoryGraph->GarphObjects[i]);
					if (ObjectWithScenObject)
					{
						ObjectWithScenObject->SetScenObjectRealPointers();
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

		for (int i = 0; i < StoryGraph->GarphObjects.Num(); i++)
		{
			
			if (UStoryGraphObjectWithScenObject* ObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(StoryGraph->GarphObjects[i]))
			{
				
				TArray<IStoryScenObject *> ScenObjects;
				ObjectWithScenObject->GetScenObjects(ScenObjects);
				
				for (int j = 0; j < ScenObjects.Num(); j++)
				{
					
					ScenObjects[j]->OwningStoryGraphObject.Add((UStoryGraphObjectWithScenObject*)StoryGraph->GarphObjects[i]);
					

				}
								
			}
		}

		USaveGameInstance* SaveGameInstance = Cast<USaveGameInstance>(GetGameInstance());

		if (!(SaveGameInstance && SaveGameInstance->IsLevelLoded))
		{
			StoryGraph->CreateExecutionTrees();
		}
	}
	

}



void AStoryGraphActor::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < StoryGraph->GarphObjects.Num(); i++)
	{
		
		if (UStoryGraphObjectWithScenObject* ObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(StoryGraph->GarphObjects[i]))
		{
			ObjectWithScenObject->InitializeObject(); //Set initial active state
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

	if (Package != NULL)
	{
		const bool bIsDirty = Package->IsDirty();

		if (!bIsDirty)
		{
			Package->SetDirtyFlag(true);
		}

		Package->PackageMarkedDirtyEvent.Broadcast(Package, bIsDirty);
	}
}

