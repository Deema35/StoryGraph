// Copyright 2016 Dmitriy Pavlov

#include "StoryGraphPluginEditor.h"
#include "AssetTypeActions_StoryGraph.h"
#include "EdGraphUtilities.h"
#include "Commands_StoryGraph.h"
#include "GraphNodes_StoryGraph.h"
#include "GraphPanelNodeFactory_StoryGraph.h"
#include "DetailPanelCustomizationObject.h"
#include "DetailPanelCustomizationNodes.h"
#include "ClassIconFinder.h"
#include "NodeStyle.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FStoryGraphEditorModule"

void FStoryGraphEditorModule::StartupModule()
{
	FNodeStyle::Initialize();


	//Registrate asset actions for StoryGraph
	FAssetTypeActions_StoryGraph::RegistrateCustomPartAssetType();

	// Register custom graph nodes
	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory = MakeShareable(new FGraphPanelNodeFactory_StoryGraph);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);

	//Registrate ToolBarCommand for custom graph
	FCommands_StoryGraph::Register();


	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Registrate custom detail panel for Story objects
	int i = 1; // skip non type
	while (i < GetNumberEnums("EStoryObjectType"))
	{
		switch ((EStoryObjectType)i)
		{
		case EStoryObjectType::Character:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphCharacterDetail::MakeInstance));
			break;

		case EStoryObjectType::PlaceTrigger:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphPlaceTriggerDetail::MakeInstance));
			break;

		case EStoryObjectType::InventoryItem:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphInventoryItemDetail::MakeInstance));
			break;

		case EStoryObjectType::Others:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphOthersDetail::MakeInstance));
			break;
		default:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphObjectDetail::MakeInstance));
		}
		i++;
	}

	//Registrate custom detail panel for Nods
	i = 0;
	while (i < GetNumberEnums("ENodeType"))
	{
		switch ((ENodeType)i)
		{
		case ENodeType::DialogStart:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FDialogStartNodeDetail::MakeInstance));
			break;

		case ENodeType::DialogNode:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FDialogNodeDetail::MakeInstance));
			break;


		case ENodeType::GetStoryGraphObjectState:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FGetStoryGraphObjectStateNodeDetail::MakeInstance));

			break;

		case ENodeType::AddDialogFromDialog:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FAddDialogFromDialogDetail::MakeInstance));
			break;

		case ENodeType::AddDialog:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FAddDialogNodeDetail::MakeInstance));
			break;

		case ENodeType::SetInventoryItemState:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FSetInventoryItemStateNodeDetail::MakeInstance));
			break;

		case ENodeType::AddQuestPhase:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FAddQuestPhaseNodeDetail::MakeInstance));
			break;

		case ENodeType::QuestStart:

			PropertyModule.RegisterCustomClassLayout(
				FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
				FOnGetDetailCustomizationInstance::CreateStatic(&FQuestStarNodeDetail::MakeInstance));
			break;

		case ENodeType::MessageStart:
		case ENodeType::Message:
		case ENodeType::MessageExit:
		case ENodeType::MessageEnd:
		case ENodeType::AddMessageBranch:
		case ENodeType::SetInventoryItemStateFromMessage:

			break;

		default:

			switch (UCustomNodeBase::GetInsertNodeType((ENodeType)i))
			{
			case EInsertNodeType::StoryGraphStandalone:


				PropertyModule.RegisterCustomClassLayout(
					FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
					FOnGetDetailCustomizationInstance::CreateStatic(&FCustomNodeBaseDetail::MakeInstance));
				break;

			case EInsertNodeType::StoryGraphDependent:

				PropertyModule.RegisterCustomClassLayout(
					FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
					FOnGetDetailCustomizationInstance::CreateStatic(&FStoryGraphDependedNodeDetail::MakeInstance));
				break;

			case EInsertNodeType::DialogGraphStandalone:
			case EInsertNodeType::MessageGraphStandalone:


				PropertyModule.RegisterCustomClassLayout(
					FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
					FOnGetDetailCustomizationInstance::CreateStatic(&FDialogNodeBaseDetail::MakeInstance));
				break;

			case EInsertNodeType::DialogGraphDependent:

				PropertyModule.RegisterCustomClassLayout(
					FName(*UCustomNodeBase::GetClassFromNodeType((ENodeType)i)->GetName()),
					FOnGetDetailCustomizationInstance::CreateStatic(&FDialogDependedNodeDetail::MakeInstance));
				break;
			}
		}

		i++;
	}
}

void FStoryGraphEditorModule::ShutdownModule()
{
	FNodeStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryGraphEditorModule, StoryGraphPluginEditor);
