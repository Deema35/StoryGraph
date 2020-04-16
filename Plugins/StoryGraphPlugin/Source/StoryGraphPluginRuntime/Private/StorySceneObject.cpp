// Copyright 2016 Dmitriy Pavlov

#include "StorySceneObject.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "ObjectRecord.h"


//IStorySceneObject.......................................................
void IStorySceneObject::RefreshSceneObjectsActive()
{
	bool IsActive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		if (OwningStoryGraphObject[i]->IsSceneObjectActive)
		{
			IsActive = true;
			break;
		}
	}
	EnableObjectOnMap(IsActive);
}


//ACharacter_StoryGraph........................................................................
ACharacter_StoryGraph::ACharacter_StoryGraph()
{
}

void ACharacter_StoryGraph::ChangeState(ECharacterStates NewState)
{
	SetStoryGraphObjectState((int)NewState);
}

void ACharacter_StoryGraph::OpenDialog()
{
	if (OwningStoryGraphObject.Num() > 0)
	{
		AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD());

		if (HUD)
		{
			HUD->OpenDialogOrOpenPlaceTriggerMessages(OwningStoryGraphObject);
		}
		else
		{
			UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
		}
	}
}

void ACharacter_StoryGraph::EnableObjectOnMap(bool IsEnabled)
{
	IsEnabled = IsEnabled;
	SetActorHiddenInGame(!IsEnabled);
	SetActorEnableCollision(IsEnabled);
	SetActorTickEnabled(IsEnabled);
}

void ACharacter_StoryGraph::SendMessageToSceneObject(FString Message)
{
	GetMessageFromStoryGraph(Message);
}

FText ACharacter_StoryGraph::GetObjectName()
{
	return OwningStoryGraphObject.Num() > 0 ? OwningStoryGraphObject[0]->ObjName : FText::FromString("Non");
}


void ACharacter_StoryGraph::SetStoryGraphObjectState(int NewState)
{
	if (GetWorld()->GetFirstPlayerController())
	{
		for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
		{
			if (OwningStoryGraphObject[i])
			{
				OwningStoryGraphObject[i]->SetCurrentState(NewState);
			}
		}
	}
}

//ASceneObjectActor_StoryGraph....................................................

void ASceneObjectActor_StoryGraph::EnableObjectOnMap(bool IsEnabled)
{
	IsEnabled = IsEnabled;
	SetActorHiddenInGame(!IsEnabled);
	SetActorEnableCollision(IsEnabled);
	SetActorTickEnabled(IsEnabled);
}

void ASceneObjectActor_StoryGraph::SendMessageToSceneObject(FString Message)
{
	GetMessageFromStoryGraph(Message);
}

FText ASceneObjectActor_StoryGraph::GetObjectName()
{
	return OwningStoryGraphObject.Num() > 0 ? OwningStoryGraphObject[0]->ObjName : FText::FromString("Non");
}

void ASceneObjectActor_StoryGraph::SetStoryGraphObjectState(int NewState)
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
		{
			if (OwningStoryGraphObject[i])
			{
				OwningStoryGraphObject[i]->SetCurrentState(NewState);
			}
		}
	}
}

//APlaceTrigger_StoryGraph......................................................................


void APlaceTrigger_StoryGraph::ChangeState(EPlaceTriggerStates NewState)
{
	SetStoryGraphObjectState((int)NewState);
}

void APlaceTrigger_StoryGraph::Activate()
{
	TArray<UStoryGraphObjectWithSceneObject*> AdvanceInteractiveTriggers;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		switch (((UStoryGraphPlaceTrigger*)OwningStoryGraphObject[i])->PlaceTriggerType)
		{
		case EPlaceTriggerType::UnInteractive:

			break;

		case EPlaceTriggerType::Interactive:

			OwningStoryGraphObject[i]->SetCurrentState((int)EPlaceTriggerStates::Active);
			break;

		case EPlaceTriggerType::AdvanceInteractive:


			AdvanceInteractiveTriggers.Add(OwningStoryGraphObject[i]);
			break;
		}
	}
	if (AdvanceInteractiveTriggers.Num() > 0)
	{
		if (GetWorld())
		{
			AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD());

			if (HUD)
			{
				HUD->OpenDialogOrOpenPlaceTriggerMessages(AdvanceInteractiveTriggers);
			}
			else
			{
				UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning,
				       TEXT("Your HUD should inherit AHUD_StoryGraph class"));
			}
		}
	}
}

EPlaceTriggerType APlaceTrigger_StoryGraph::GetPlaceTriggerType()
{
	bool Interactive = false;
	bool AdvanceInteractive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		switch (((UStoryGraphPlaceTrigger*)OwningStoryGraphObject[i])->PlaceTriggerType)
		{
		case EPlaceTriggerType::UnInteractive:

			break;

		case EPlaceTriggerType::Interactive:

			Interactive = true;
			break;

		case EPlaceTriggerType::AdvanceInteractive:

			AdvanceInteractive = true;
			break;
		}
	}

	if (AdvanceInteractive) return EPlaceTriggerType::AdvanceInteractive;

	if (Interactive) return EPlaceTriggerType::Interactive;

	return EPlaceTriggerType::UnInteractive;
}


//AInventoryItem_StoryGraph.............................................................................


void AInventoryItem_StoryGraph::PickUp()
{
	SetStoryGraphObjectState((int)EInventoryItemStates::InInventory + 1);

	EnableObjectOnMap(false);
}


void AInventoryItem_StoryGraph::RefreshSceneObjectsActive()
{
	bool IsActive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		if (OwningStoryGraphObject[i]->IsSceneObjectActive)
		{
			IsActive = true;
			break;
		}
	}
	if (IsActive && OwningStoryGraphObject[0]->GetCurrentState() != (int)EInventoryItemStates::OnLevel + 1)
	{
		IsActive = false;
	}
	EnableObjectOnMap(IsActive);
}
