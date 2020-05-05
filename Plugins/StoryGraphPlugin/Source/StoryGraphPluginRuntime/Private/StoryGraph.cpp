// Copyright 2016 Dmitriy Pavlov

#include "StoryGraph.h"
#include "CustomNodes.h"
#include "ExecutionTree.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "StoryGraphObject.h"
#include "StoryGraphWidget.h"

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