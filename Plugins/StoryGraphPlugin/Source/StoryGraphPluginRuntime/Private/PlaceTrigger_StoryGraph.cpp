#include "PlaceTrigger_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "HUD_StoryGraph.h"

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