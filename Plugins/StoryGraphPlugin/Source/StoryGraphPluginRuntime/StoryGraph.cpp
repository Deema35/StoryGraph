// Copyright 2016 Dmitriy Pavlov
#include "StoryGraphPluginRuntime.h"
#include "StoryGraph.h"
#include "CustomNods.h"
#include "EdGraph/EdGraph.h"
#include "StoryGraphObject.h"
#include "StoryScenObject.h"
#include "HUD_StoryGraph.h"
#include "StoryGraphWiget.h"

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


//FExecutionTree...................................................................
void FExecutionTree::Refresh()
{
	TArray<UCustomNodeBase*> ChildNodes;
	
	TArray<UStoryVerticalNodeBase*> NodsForAdd;
	bool AllNodsPerfomed;

	if (QuestStartNode->pGraphObject->GetCurentState() == (int)EQuestStates::Active ||
		QuestStartNode->pGraphObject->GetCurentState() == (int)EQuestStates::UnActive)
	{

		if (PredActiveNodesBuffer.Num() == 0)
		{
			PredActiveNodesBuffer.Add(QuestStartNode);
		}

		AllNodsPerfomed = false;

		while (!AllNodsPerfomed)
		{

			AllNodsPerfomed = true;
			
			for (int j = 0; j < PredActiveNodesBuffer.Num(); j++)
			{
				EPerformNodeResult PerformResult = PredActiveNodesBuffer[j]->PerformNode();
			
				if (PerformResult == EPerformNodeResult::Sucssed)
				{
					PredActiveNodesBuffer[j]->ResetUnPerformBrunch(); //if we have already perform node, we must reset others ways for this node
					PredActiveNodesBuffer[j]->GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);
					if (ChildNodes.Num() == 0)
					{
						
						QuestStartNode->pGraphObject->SetCurentState((int)EQuestStates::Complite); //We find end of tree an quest complite
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
				
			PredActiveNodesBuffer.Empty();
			PredActiveNodesBuffer.Append(NodsForAdd);
			NodsForAdd.Empty();

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
		ExecutionTrees.Add(FExecutionTree());
		ExecutionTrees[i].QuestStartNode = QuestStartNodes[i];
	}
	
}

void UStoryGraph::RefreshExecutionTrees(bool NeedRefreshQuestsPhase)
{

	for (int i = 0; i < ExecutionTrees.Num(); i++)
	{
		ExecutionTrees[i].Refresh();
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
		for (int j = 0; j <  ExecutionTrees[i].PredActiveNodesBuffer.Num(); j++)
		{
			if (ExecutionTrees[i].PredActiveNodesBuffer[j]->pQuestPhase)
			{
				ExecutionTrees[i].PredActiveNodesBuffer[j]->pQuestPhase->QuestPhaseState = EQuestPhaseState::Active;
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
	
	if (Radar)
	{
		Radar->RefreshTargets(RadarTargets);
	}
}

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
		
		for (int i = 0; i < StoryGraph->GarphObjects.Num(); i++)
		{
			UStoryGraphObjectWithScenObject* ObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(StoryGraph->GarphObjects[i]);
			if (ObjectWithScenObject)
			{
				
				TArray<IStoryScenObject *> ScenObjects;
				ObjectWithScenObject->GetScenObjects(ScenObjects);
				
				for (int j = 0; j < ScenObjects.Num(); j++)
				{
					
					ScenObjects[j]->OwningStoryGraphObject.Add((UStoryGraphObjectWithScenObject*)StoryGraph->GarphObjects[i]);
					

				}


				ObjectWithScenObject->SetActiveStateOfScenObjects(); //Set initial active state
			}
		}
		StoryGraph->CreateExecutionTrees();
	}
	
	
}


void AStoryGraphActor::BeginPlay()
{
	Super::BeginPlay();
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