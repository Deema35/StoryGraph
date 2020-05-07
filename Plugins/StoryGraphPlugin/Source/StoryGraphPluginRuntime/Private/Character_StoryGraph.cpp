#include "Character_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "HUD_StoryGraph.h"

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

void ACharacter_StoryGraph::EnableObjectOnMap(const bool IsEnabledOnMap)
{
	this->IsEnabled = IsEnabledOnMap;
	SetActorHiddenInGame(!IsEnabledOnMap);
	SetActorEnableCollision(IsEnabledOnMap);
	SetActorTickEnabled(IsEnabledOnMap);
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