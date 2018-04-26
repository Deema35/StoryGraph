// Copyright 2016 Dmitriy Pavlov

#include "StoryScenObject.h"
#include "HUD_StoryGraph.h"
#include "ObjectrRecord.h"
#include "LogCategoryRutime.h"


//IStoryScenObject.......................................................
void IStoryScenObject::RefreshScenObjectActive()
{
	bool IsActive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		if (OwningStoryGraphObject[i]->IsScenObjectActive)
		{
			IsActive = true;
			break;
		}
	}
	EnabelObjectOnMap(IsActive);
}



//ACharecter_StoryGraph........................................................................
ACharecter_StoryGraph::ACharecter_StoryGraph()
{
	
}
void ACharecter_StoryGraph::ChangeState(ECharecterStates NewState)
{
	SetStoryGraphObjectState((int)NewState);
}

void ACharecter_StoryGraph::OpenDialog()
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

void ACharecter_StoryGraph::EnabelObjectOnMap(bool Enabel)
{
	IsEnabel = Enabel;
	SetActorHiddenInGame(!Enabel);
	SetActorEnableCollision(Enabel);
	SetActorTickEnabled(Enabel);
}

void ACharecter_StoryGraph::SendMessageToScenObject(FString Message)
{
	GetMessegeFromStoryGraph(Message);
}

FText ACharecter_StoryGraph::GetObjectName()
{
	return OwningStoryGraphObject.Num() > 0 ? OwningStoryGraphObject[0]->ObjName : FText::FromString("Non");
}



void ACharecter_StoryGraph::SetStoryGraphObjectState(int NewState)
{

	if (GetWorld()->GetFirstPlayerController())
	{
		for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
		{
			if (OwningStoryGraphObject[i])
			{
				OwningStoryGraphObject[i]->SetCurentState(NewState);
			}
		}
	}

}

//AScenObjectActor_StoryGraph....................................................

void AScenObjectActor_StoryGraph::EnabelObjectOnMap(bool Enabel)
{
	IsEnabel = Enabel;
	SetActorHiddenInGame(!Enabel);
	SetActorEnableCollision(Enabel);
	SetActorTickEnabled(Enabel);
}

void AScenObjectActor_StoryGraph::SendMessageToScenObject(FString Message)
{
	GetMessegeFromStoryGraph(Message);
}

FText AScenObjectActor_StoryGraph::GetObjectName()
{
	return OwningStoryGraphObject.Num() > 0 ? OwningStoryGraphObject[0]->ObjName : FText::FromString("Non");
}

void AScenObjectActor_StoryGraph::SetStoryGraphObjectState(int NewState)
{

	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
		{
			if (OwningStoryGraphObject[i])
			{
				OwningStoryGraphObject[i]->SetCurentState(NewState);
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
	TArray<UStoryGraphObjectWithScenObject*> AdvanceInteractiveTriggers;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		switch (((UStoryGraphPlaceTrigger*)OwningStoryGraphObject[i])->PlaceTriggerType)
		{
		case EPlaceTriggerType::UnInteractive:

			break;

		case EPlaceTriggerType::Interactive:

			OwningStoryGraphObject[i]->SetCurentState((int)EPlaceTriggerStates::Active);
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
				UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
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
	SetStoryGraphObjectState((int)EInventoryItemeStates::InInventory + 1);
	
	EnabelObjectOnMap(false);
		
}


void AInventoryItem_StoryGraph::RefreshScenObjectActive()
{
	
	bool IsActive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		if (OwningStoryGraphObject[i]->IsScenObjectActive)
		{
			IsActive = true;
			break;
		}
	}
	if (IsActive && OwningStoryGraphObject[0]->GetCurentState() != (int)EInventoryItemeStates::OnLevel + 1)
	{
		IsActive = false;
	}
	EnabelObjectOnMap(IsActive);
}



