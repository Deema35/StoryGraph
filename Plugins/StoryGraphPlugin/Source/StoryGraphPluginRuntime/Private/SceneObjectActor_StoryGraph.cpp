#include "SceneObjectActor_StoryGraph.h"

void ASceneObjectActor_StoryGraph::EnableObjectOnMap(const bool IsEnabled)
{
	SetActorHiddenInGame(!IsEnabled);
	SetActorEnableCollision(IsEnabled);
	SetActorTickEnabled(IsEnabled);
}

void ASceneObjectActor_StoryGraph::SendMessageToSceneObject(const FString Message)
{
	GetMessageFromStoryGraph(Message);
}

FText ASceneObjectActor_StoryGraph::GetObjectName()
{
	return OwningStoryGraphObject.Num() > 0 ? OwningStoryGraphObject[0]->ObjName : FText::FromString("Non");
}

void ASceneObjectActor_StoryGraph::SetStoryGraphObjectState(const int NewState)
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