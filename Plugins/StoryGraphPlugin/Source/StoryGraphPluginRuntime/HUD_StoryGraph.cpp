// Copyright 2016 Dmitriy Pavlov


#include "HUD_StoryGraph.h"
#include "StoryGraphObject.h"
#include "CustomNods.h"
#include "StoryGraph.h"
#include "StoryGraphWiget.h"
#include "SaveGameInstance.h"
#include "LogCategoryRutime.h"
#include "EngineUtils.h"

void AHUD_StoryGraph::OpenDialogOrOpenPlaceTriggerMessages(TArray<UStoryGraphObjectWithScenObject*>& OwningObjects)
{
	CurrentStoryGraphObjects.Empty();

	CurrentStoryGraphObjects.Append(OwningObjects);

	TArray<UDialogObject*> Dialogs;

	GetRootDialogs(Dialogs);
	
	if (CurrentStoryGraphObjects.Num() > 0)
	{
		
		if (Dialogs.Num() > 0)
		{
			
			if (UStoryGraphCharecter* Character = Cast<UStoryGraphCharecter>(CurrentStoryGraphObjects[0]))
			{
				OpenDialogEvent(true);
			}
			else if (UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentStoryGraphObjects[0]))
			{
				OpenPlaceTriggerMessagesEvent(true);
			}
		}
		else
		{
			for (int i = 0; i < CurrentStoryGraphObjects.Num(); i++)
			{
				if (UStoryGraphCharecter* Character = Cast<UStoryGraphCharecter>(CurrentStoryGraphObjects[i]))
				{
					if (Character->DefaultAnswer.ToString().Len() > 0)
					{
						GameScreen->AddDefaultAnswerOnScreen(Character->DefaultAnswer);
						break;
					}
					else
					{
						UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Character %s dont have default answer"), *Character->ObjName.ToString());
					}
				}
				else if (UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentStoryGraphObjects[i]))
				{
					if (PlaceTrigger->DefaultAnswer.ToString().Len() > 0)
					{
						GameScreen->AddDefaultAnswerOnScreen(PlaceTrigger->DefaultAnswer);
						break;
					}
					else
					{
						UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("PlaceTrigger %s dont have default answer"), *PlaceTrigger->ObjName.ToString());
					}
				}
			}
		}
	}
	
}


void AHUD_StoryGraph::GetRootDialogs(TArray<UDialogObject*>& Dialogs)
{

	for (int i = 0; i < CurrentStoryGraphObjects.Num(); i++)
	{
		UStoryGraphCharecter* Character = Cast<UStoryGraphCharecter>(CurrentStoryGraphObjects[i]);
		UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentStoryGraphObjects[i]);
		if (Character)
		{
			for (int j = 0; j < Character->GarphNods.Num(); j++)
			{
				if (UDialogStartNode* DialogStartNode = Cast<UDialogStartNode>(Character->GarphNods[j]))
				{
					if (DialogStartNode->IsActive)
					{
						Dialogs.Add(DialogStartNode->Dialogs[0]);
					}
				}
			}
		}
		else if (PlaceTrigger)
		{
			for (int j = 0; j < PlaceTrigger->GarphNods.Num(); j++)
			{
				if (UDialogStartNode* DialogStartNode = Cast<UDialogStartNode>(PlaceTrigger->GarphNods[j]))
				{
					if (DialogStartNode->IsActive)
					{
						Dialogs.Add(DialogStartNode->Dialogs[0]);
					}
				}
			}
		}
	}
	
	//Sort predicate
	auto Predicate = [](const UDialogObject& Dialog1, const UDialogObject& Dialog2)
	{
		return ((UDialogStartNode*)Dialog1.DialogNode)->DialogPriority < ((UDialogStartNode*)Dialog2.DialogNode)->DialogPriority;
	};
	//Sort dialog by priority
	if (Dialogs.Num() > 0)
	{
		Sort(&Dialogs[0], Dialogs.Num(), Predicate);
	}

}


void AHUD_StoryGraph::GetNextDialogFromBrunch(UCustomNodeBase* Brunch, TArray<UDialogObject*>& Dialogs)
{
	Dialogs.Empty();

	if (!Brunch)
	{
		return;
	}
	while (true)
	{
		
		switch (Brunch->PerformNode())
		{
		case EPerformNodeResult::NodeHaveDialogs:
			
			Dialogs.Append(((UDialogNodeBase*)Brunch)->Dialogs);
			
			
			
			return;

		case EPerformNodeResult::Fail:
			return;

		case EPerformNodeResult::Sucssed:

			Brunch = Brunch->GetFistChildNode();

			if (!Brunch)
			{
				return;
			}
			break;

		default:

			return;

		}

	}
}

void  AHUD_StoryGraph::ChangeLocalization(FString target)
{
	FInternationalization::Get().SetCurrentCulture(target);
}

FString  AHUD_StoryGraph::GetCurrentLocalization()
{
	FCultureRef CultRef = FInternationalization::Get().GetCurrentCulture();
	return CultRef->GetName();
}


void AHUD_StoryGraph::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if (GetGameInstance())
	{
		if (USaveGameInstance* SaveGameInstance = Cast<USaveGameInstance>(GetGameInstance()))
		{
			SaveGameInstance->LoadGameContinue();
		}


		else UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("You must set GameIstans clas as USaveGameInstance"));
	}
}

void AHUD_StoryGraph::BeginPlay()
{
	Super::BeginPlay();
	USaveGameInstance* SaveGameInstance = Cast<USaveGameInstance>(GetGameInstance());
	if (SaveGameInstance)
	{
		SaveGameInstance->LoadCharacter(); // Load our character
	}
	
}

void AHUD_StoryGraph::GetStoryGraphs(TArray<class UStoryGraph*>& StoryGraphs_)
{
	if (GWorld && !StoryGraphFind)
	{

		StoryGraphs.Empty();
		for (TActorIterator<AStoryGraphActor> ActorItr(GWorld); ActorItr; ++ActorItr)
		{
			if (ActorItr->StoryGraph)
			{
				StoryGraphs.Add(ActorItr->StoryGraph);
			}
		}
		StoryGraphFind = true;
	}
	StoryGraphs_.Empty();
	StoryGraphs_.Append(StoryGraphs);
}
