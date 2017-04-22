// Copyright 2016 Dmitriy Pavlov

#include "AssetTypeActions_StoryGraph.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "AssetEditor_StoryGraph.h"
#include "StoryGraph.h"
#include "Commands_StoryGraph.h"


UClass* FAssetTypeActions_StoryGraph::GetSupportedClass() const
{
	return UStoryGraphBlueprint::StaticClass();
}

FText FAssetTypeActions_StoryGraph::GetAssetDescription(const FAssetData & AssetData) const
{
	return FText::FromString("It contains quests and dialogs. Drop on scen to enabel they in game.");
}


void FAssetTypeActions_StoryGraph::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	for (int i = 0; i < InObjects.Num(); i++)
	{
		UStoryGraphBlueprint* PropData = Cast<UStoryGraphBlueprint>(InObjects[i]);
		if (PropData && PropData->StoryGraph)
		{
			
			MenuBuilder.AddMenuEntry(
				FText::FromString("Add to scene"),
				FText::FromString("Add Storygraph to scene if it dosn't exist."),
				FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Duplicate"),
				FUIAction(
					FExecuteAction::CreateSP(this, &FAssetTypeActions_StoryGraph::AddToScen, PropData)
				)
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Remove from scene"),
				FText::FromString("Remove Storygraph from scene."),
				FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.Delete"),
				FUIAction(
					FExecuteAction::CreateSP(this, &FAssetTypeActions_StoryGraph::RemoveFromScen, PropData)
				)
			);
		}
	}
}



void FAssetTypeActions_StoryGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (int i = 0; i < InObjects.Num(); i++)
	{
		UStoryGraphBlueprint* PropData = Cast<UStoryGraphBlueprint>(InObjects[i]);
		if (PropData && PropData->StoryGraph)
		{
			if (!PropData->StoryGraph->pAssetEditor)
			{
				TSharedRef<FAssetEditor_StoryGraph> NewCustEditor(new FAssetEditor_StoryGraph());
				PropData->StoryGraph->pAssetEditor = &NewCustEditor.Get();
				NewCustEditor->InitAssetEditor_StoryGraph(Mode, EditWithinLevelEditor, PropData);
				//FAssetEditorManager::Get().NotifyAssetOpened(PropData, PropData->StoryGraph->pAssetEditor);
			}
		}
	}
}

void FAssetTypeActions_StoryGraph::RegistrateCustomPartAssetType()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_StoryGraph));

}

void FAssetTypeActions_StoryGraph::AddToScen(UStoryGraphBlueprint* PropData)
{
	bool IsObjectExist = false;
	for (TActorIterator<AStoryGraphActor> ActorItr(GWorld); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass()->ClassGeneratedBy == PropData)
		{
			IsObjectExist = true;
		}
	}
	if (!IsObjectExist)
	{
		FActorSpawnParameters SpawnInfo;
		AActor* StoryGraph = GWorld->SpawnActor<AStoryGraphActor>(PropData->GeneratedClass, SpawnInfo);
		StoryGraph->SetFolderPath(FName("Story"));
	}
}

void FAssetTypeActions_StoryGraph::RemoveFromScen(UStoryGraphBlueprint* PropData)
{
	for (TActorIterator<AStoryGraphActor> ActorItr(GWorld); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass()->ClassGeneratedBy == PropData)
		{
			ActorItr->Destroy();
		}
	}
	
}

