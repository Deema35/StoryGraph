// Copyright 2016 Dmitriy Pavlov
#pragma warning(disable: 4458)


#include "AssetEditor_StoryGraph.h"
#include "EditorStyleSet.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "GraphSchema_StoryGraph.h"
#include "SDockTab.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "CustomNods.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "Commands_StoryGraph.h"
#include "BlueprintEditorModule.h"
#include "PropertyEditorModule.h"
#include "CustomNods.h"
#include "StoryGraph.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "SGraphActionMenu.h"
#include "SSearchBox.h"
#include "SGraphNode.h"
#include "SSCSEditor.h"
#include "StoryGraphObject.h"
#include "DetailLayoutBuilder.h"
#include "GraphSchemaAction_StoryGraph.h"
#include "GraphPaletteItem_StoryGraph.h"
#include "DragDropAction_StoryGraph.h"
#include "NotificationManager.h"
#include "StoryScenObject.h"
#include "Editor/UnrealEd/Private/Toolkits/AssetEditorCommonCommands.h"
#include "Graph_StoryGraph.h"
#include "ProxyNods.h"
#include "Runtime/XmlParser/Public/XmlParser.h"
#include "Runtime/XmlParser/Public/XmlNode.h"
#include "Runtime/XmlParser/Public/XmlFile.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "LogCategoryEditor.h"
#include "EngineUtils.h"
#include "HAL/PlatformApplicationMisc.h"


const FName CustomEditorAppName = FName(TEXT("CustomEditorApp"));

FAssetEditor_StoryGraph::FAssetEditor_StoryGraph() : CurrentSelectedObject(NULL), CurrentSelectedActionMenuObject(NULL), AssetObject(NULL), EditedObject(NULL)
{
	TabForegroundedDelegateHandle = FGlobalTabmanager::Get()->OnTabForegrounded_Subscribe(FOnActiveTabChanged::FDelegate::CreateRaw(this, &FAssetEditor_StoryGraph::OnTabForegrounded));
}

FAssetEditor_StoryGraph::~FAssetEditor_StoryGraph()
{
	FGlobalTabmanager::Get()->OnTabForegrounded_Unsubscribe(TabForegroundedDelegateHandle);
	EditedObject->pAssetEditor = NULL;
}




const FName FCustomEditorTabs::DetailsID(TEXT("Details"));
const FName FCustomEditorTabs::ViewportID(TEXT("Viewport"));
const FName FCustomEditorTabs::ActionMenuID(TEXT("ActionMenu"));

FName FAssetEditor_StoryGraph::GetToolkitFName() const
{
	return FName("SyoryGraph Editor");
	
}

FText FAssetEditor_StoryGraph::GetBaseToolkitName() const
{
	return FText::FromString("SyoryGraph EditorToolkitName");
}

FLinearColor FAssetEditor_StoryGraph::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_StoryGraph::GetWorldCentricTabPrefix() const
{
	return TEXT("SyoryGraph Editor");
}

FText FAssetEditor_StoryGraph::GetToolkitName() const
{
	check(AssetObject != NULL);

	return GetLabelForObject(AssetObject);
}

void FAssetEditor_StoryGraph::OnTabForegrounded(TSharedPtr<SDockTab> ForegroundedTab, TSharedPtr<SDockTab> BackgroundedTab)
{
	//UE_LOG(StoryGraphEditor, Warning, TEXT("TabForegrounded"));
	TSharedRef<SGraphEditor> GraphEditorLoc = StaticCastSharedRef<SGraphEditor>(ForegroundedTab->GetContent());
	
	if (ForegroundedTab->GetTabLabel().ToString() == "Story Graph" || ForegroundedTab->GetTabLabel().ToString().Find("DG_") != -1 || ForegroundedTab->GetTabLabel().ToString().Find("MS_") != -1)
	{
		OnGraphEditorFocused(GraphEditorLoc);
	}	
}

void FAssetEditor_StoryGraph::InitAssetEditor_StoryGraph(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	
	//Create graph editor
	AssetObject = CastChecked<UStoryGraphBlueprint>(ObjectToEdit);
	EditedObject = AssetObject->StoryGraph;
	UEdGraph_StoryGraph* CustGraph = AssetObject->FindGraph(EditedObject);
	if (!CustGraph)
	{
		CustGraph = NewObject<UEdGraph_StoryGraph>(AssetObject, UEdGraph_StoryGraph::StaticClass(), NAME_None, RF_Transactional);
		CustGraph->Schema = UEdGraphSchema_StoryGraph::StaticClass();
		CustGraph->StoryGraph = EditedObject;
		AssetObject->AddGraph(CustGraph);
		CustGraph->GraphOwner = EditedObject;
	}
	
	SAssignNew(FilterBox, SSearchBox)
		.OnTextChanged(this, &FAssetEditor_StoryGraph::OnFilterTextChanged);

	
	GraphEditor = CreateGraphEditorWidget(CustGraph,"Story Graph");
	GraphActionMenu = CreateActionMenuWidget();

	

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("CustomEditor_Layout_3")//Если добовлять новые панели нужно менять название слоя
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.2f)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FCustomEditorTabs::ActionMenuID, ETabState::OpenedTab)

				)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(1.0f)
					->SetHideTabWell(true)
					->AddTab(FCustomEditorTabs::ViewportID, ETabState::OpenedTab)

					)
				->Split
				(
					
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FCustomEditorTabs::DetailsID, ETabState::OpenedTab)
					
				)


				)

			);

	BindCommands();
	
	InitAssetEditor(Mode, InitToolkitHost, CustomEditorAppName, StandaloneDefaultLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, EditedObject);
	OnGraphEditorFocused(GraphEditor.ToSharedRef());

	
}
	
	

void FAssetEditor_StoryGraph::BindCommands()
{
	ToolkitCommands = MakeShareable(new FUICommandList);

	ToolkitCommands->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::DeleteStoryGraphObject));
	ToolkitCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::OnMyRequestRenameOnActionNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanRequestRenameOnActionNode));

	ToolbarExtender = MakeShareable(new FExtender);

	ToolkitCommands->MapAction(
		FCommands_StoryGraph::Get().SaveAsset,
		FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::SaveAsset_Execute),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanSaveAsset));
	
	ToolkitCommands->MapAction(
		FCommands_StoryGraph::Get().FindInContentBrowser,
		FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::FindInContentBrowser_Execute));

	ToolkitCommands->MapAction(FCommands_StoryGraph::Get().CheckObjects, FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CompilStoryObjects));

	ToolkitCommands->MapAction(FCommands_StoryGraph::Get().ExportInXML, FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::ExportInXMLFile));

	ToolkitCommands->MapAction(FCommands_StoryGraph::Get().ImportFromXML, FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::ImportFromXMLFile));

	ToolkitCommands->MapAction(FCommands_StoryGraph::Get().UnlinkAllObjects, FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::UnlinkAllStoryGraphObjects));

	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::Before, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FAssetEditor_StoryGraph::AddToolbarExtension));
	AddToolbarExtender(ToolbarExtender);
}



void FAssetEditor_StoryGraph::AddToolbarExtension(FToolBarBuilder &builder)
{
	
	FSlateIcon IconBrushSaveAsset = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.SaveAsset", "AssetEditor.SaveAsset.Small");
	FSlateIcon IconBrushFindInContentBrowser = FSlateIcon(FEditorStyle::GetStyleSetName(), "SystemWideCommands.FindInContentBrowser", "SystemWideCommands.FindInContentBrowser.Small");
	FSlateIcon IconBrushCheckObjects = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.CompileBlueprint", "Kismet.CompileBlueprint");
	FSlateIcon IconBrushExportInXML = FSlateIcon(FEditorStyle::GetStyleSetName(), "FontEditor.ExportAllPages", "FontEditor.ExportAllPages.Small");
	FSlateIcon IconBrushImportFromXML = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	FSlateIcon IconBrushUnlinkAllObjects = FSlateIcon(FEditorStyle::GetStyleSetName(), "Cascade.DeleteLOD", "Cascade.DeleteLOD.Small");
	
	builder.AddToolBarButton(FCommands_StoryGraph::Get().CheckObjects, NAME_None,
		FText::FromString("Compile"),
		FText::FromString("Compile story graph"),
		TAttribute<FSlateIcon>(this, &FAssetEditor_StoryGraph::GetStatusImage)
		, NAME_None);
	builder.AddToolBarButton(FCommands_StoryGraph::Get().SaveAsset, NAME_None, FText::FromString("Save Asset"), FText::FromString("Save Asset"), IconBrushSaveAsset, NAME_None);
	builder.AddToolBarButton(FCommands_StoryGraph::Get().FindInContentBrowser, NAME_None, FText::FromString("Find in CB"), FText::FromString("Find in Content Browser..."), IconBrushFindInContentBrowser, NAME_None);
	builder.AddToolBarButton(FCommands_StoryGraph::Get().ExportInXML, NAME_None, FText::FromString("Export in XML"), FText::FromString("Export in XML file"), IconBrushExportInXML, NAME_None);
	builder.AddToolBarButton(FCommands_StoryGraph::Get().ImportFromXML, NAME_None, FText::FromString("Import from XML"), FText::FromString("Import from XML file"), IconBrushImportFromXML, NAME_None);
	builder.AddToolBarButton(FCommands_StoryGraph::Get().UnlinkAllObjects, NAME_None, FText::FromString("Unlink objects"), FText::FromString("Unlink all StoryGraph objects"), IconBrushUnlinkAllObjects, NAME_None);
	
	
}


void FAssetEditor_StoryGraph::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromString("Custom Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(FCustomEditorTabs::ActionMenuID, FOnSpawnTab::CreateSP(this, &FAssetEditor_StoryGraph::SpawnTab_ActionMenu))
		.SetDisplayName(FText::FromName(FCustomEditorTabs::ActionMenuID))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	TabManager->RegisterTabSpawner(FCustomEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_StoryGraph::SpawnTab_Viewport))
		.SetDisplayName(FText::FromName(FCustomEditorTabs::ViewportID))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	TabManager->RegisterTabSpawner(FCustomEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_StoryGraph::SpawnTab_Details))
		.SetDisplayName(FText::FromName(FCustomEditorTabs::DetailsID))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	
}

void FAssetEditor_StoryGraph::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(FCustomEditorTabs::ActionMenuID);
	TabManager->UnregisterTabSpawner(FCustomEditorTabs::ViewportID);
	TabManager->UnregisterTabSpawner(FCustomEditorTabs::DetailsID);
}


TSharedRef<SDockTab> FAssetEditor_StoryGraph::SpawnTab_ActionMenu(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString("SGAction Menu"))
		.TabColorScale(GetTabColorScale())
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			[
				FilterBox.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			[
				GraphActionMenu.ToSharedRef()
			]

			
		];

}

TSharedRef<SDockTab> FAssetEditor_StoryGraph::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString("Story Graph"))
		.TabColorScale(GetTabColorScale())
		[
			GraphEditor.ToSharedRef()
		];

}

TSharedRef<SDockTab> FAssetEditor_StoryGraph::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;
	PropertyEditor->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_StoryGraph::OnPropertiesChanging);
	
	// Spawn the tab
	return SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			PropertyEditorRef
		];
}

void FAssetEditor_StoryGraph::GetSaveableObjects(TArray<UObject*>& OutObjects) const
{
	OutObjects.Empty();
	OutObjects.Add(AssetObject);
}



void FAssetEditor_StoryGraph::OnPropertiesChanging(const FPropertyChangedEvent& e)
{

	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	
	TArray<TWeakObjectPtr<UObject>> EditObjects;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UStoryGraphObject, Category))
	{
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UStoryGraphObject, ObjName))
	{
		
		ObjectNameChanged(CurrentSelectedActionMenuObject);

	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UStoryGraphQuest, MainQuest))
	{
		UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject);
		UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(CurrentSelectedObject);

		if (ProxyNode)
		{
			ProxyNode->CustomNode->RefreshCollor();
			((UQuestStartNode*)ProxyNode->CustomNode)->RefreshQuestOwner();
			ProxyNode->CustomNode->UpdateGraphNode();
		}
		else if (Quest)
		{
			UEdGraph_StoryGraph* CustGraph = AssetObject->FindGraph(EditedObject);
			for (int i = 0; i < CustGraph->Nodes.Num(); i++)
			{
				ProxyNode = Cast<UProxyNodeBase>(CustGraph->Nodes[i]);
				UQuestStartNode* QuestStartNode = Cast<UQuestStartNode>(ProxyNode->CustomNode);
				if (QuestStartNode)
				{
					if (QuestStartNode->pGraphObject == Quest)
					{

						QuestStartNode->RefreshCollor();
						QuestStartNode->RefreshQuestOwner();
						QuestStartNode->UpdateGraphNode();
						break;

					}
				}
			}
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UDialogStartNode, IsActive))
	{
		UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject);

		if (ProxyNode)
		{
			ProxyNode->CustomNode->RefreshCollor();
		}
		
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UAddQuestPhaseNode, IsEmpty))
	{
		UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject);

		if (ProxyNode)
		{
			if (((UAddQuestPhaseNode*)ProxyNode->CustomNode)->IsEmpty)
			{
				((UAddQuestPhaseNode*)ProxyNode->CustomNode)->QuestPhaseToAdd->Decription = FText::GetEmpty();
			}
			else
			{
				((UAddQuestPhaseNode*)ProxyNode->CustomNode)->QuestPhaseToAdd->Decription = FText::FromString("Enter text");
			}
			ProxyNode->CustomNode->NodeUpdateDelegate.ExecuteIfBound();
			ProxyNode->CustomNode->ProprtyUpdateDelegate.ExecuteIfBound();
		}

	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UAddDialogNode, Activate))
	{
		UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject);
	
		if (ProxyNode)
		{
			ProxyNode->CustomNode->RefreshCollor();
			ProxyNode->CustomNode->UpdateGraphNode();
		}

	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UStoryGraphInventoryItem, InventoryItemWithoutScenObject))
	{
		RefreshDetailPanel();
	}

	EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
}

void FAssetEditor_StoryGraph::ObjectNameChanged(UStoryGraphObject* ChangObject)
{
	
	TArray<UEdGraphNode*> NodesToRefresh;
	
	FindDependetNodsInGraph(NodesToRefresh, ChangObject);
	for (int i = 0; i < NodesToRefresh.Num(); i++)
	{
		((UProxyNodeBase*)NodesToRefresh[i])->CustomNode->NodeUpdateDelegate.ExecuteIfBound(); //Refresh node UI
	}
	GraphActionMenu->RefreshAllActions(true); //Refresh action panel
	
}

TSharedRef<SGraphEditor> FAssetEditor_StoryGraph::CreateGraphEditorWidget(UEdGraph* InGraph, FString CornerText)
{
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString(CornerText);
	
	if (!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShareable(new FUICommandList);
		{
			GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::SelectAllNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanSelectAllNodes)
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::DeleteSelectedNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanDeleteNodes)
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CopySelectedNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanCopyNodes)
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::PasteNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanPasteNodes)
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CutSelectedNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanCutNodes)
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
				FExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::DuplicateNodes),
				FCanExecuteAction::CreateSP(this, &FAssetEditor_StoryGraph::CanDuplicateNodes)
				);


		}
	}
	

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_StoryGraph::OnSelectedNodesChanged);
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_StoryGraph::OnNodeDoubleClicked);

	

	TSharedRef<SGraphEditor> _GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents)
		;

	
	return _GraphEditor;
}




// Editor Actions......................................................................................................

void FAssetEditor_StoryGraph::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> SelectedObjects;
	for (UObject* Object : NewSelection)
	{
		SelectedObjects.Add(Object);
		
	}
	if (SelectedObjects.Num() > 0)
	{
		CurrentSelectedObject = SelectedObjects[0];
		if (UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject))
		{
			PropertyEditor->SetObject(ProxyNode->CustomNode);
		}
	}
	GraphActionMenu->RefreshAllActions(true, false);

	
}

//void FAssetEditor_StoryGraph::OnNodeDoubleClicked(class UEdGraphNode* Node)
//{
//	((UCustomNodeBase*)Node)->DoubleClicke();
//	
//}

void FAssetEditor_StoryGraph::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();
	if (FocusedGraphEd.IsValid())
	{
		FocusedGraphEd->SelectAllNodes();
	}

}

void FAssetEditor_StoryGraph::ClearSelection()
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();
	if (FocusedGraphEd.IsValid())
	{
		FocusedGraphEd->ClearSelectionSet();
	}
}

bool FAssetEditor_StoryGraph::CanSelectAllNodes() const
{
	return FocusedGraphEdPtr.IsValid();
}

void FAssetEditor_StoryGraph::DeleteSelectedNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEd->GetSelectedNodes();

		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
		}

		DeleteNodes(NodesToDelete);
	}
}



bool FAssetEditor_StoryGraph::CanDeleteNodes() const
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		// If any of the nodes can be duplicated then we should allow copying
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEd->GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
		{
			UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
			if ((Node != NULL) && Node->CanUserDeleteNode())
			{
				return true;
			}
		}
	}
	return false;
}

void FAssetEditor_StoryGraph::DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete)
{
	if (NodesToDelete.Num() > 0)
	{
		for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index)
		{
			if (!NodesToDelete[Index]->CanUserDeleteNode())
			{
				continue;
			}
			// Remove custom node links
			UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(NodesToDelete[Index]);
			for (int i = 0; i < ProxyNode->CustomNode->NodePins.Num(); i++)
			{
				for (int j = 0; j < ProxyNode->CustomNode->NodePins[i].Links.Num(); j++)
				{
					ProxyNode->CustomNode->NodePins[i].Links[j]->DelitLinkToNode(ProxyNode->CustomNode);
				}
			}

			/// Remove proxy node links
			NodesToDelete[Index]->BreakAllNodeLinks();

			FBlueprintEditorUtils::RemoveNode(NULL, NodesToDelete[Index], true);
			EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
			// TODO: Process deletion in the data model
		}
	}
	
}



void FAssetEditor_StoryGraph::CopySelectedNodes()
{

	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{

		// Export the selected nodes and place the text on the clipboard
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEd->GetSelectedNodes();
		
		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
		{
			
			if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
			{
				if (!Node->CanDuplicateNode())
				{
					continue;
				}
				Node->PrepareForCopying();
			}
		}

		FString ExportedText;

		FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
		FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
	}
	// Make sure the owner remains the same for the copied node
	// TODO: Check MaterialEditor.cpp for reference

}

bool FAssetEditor_StoryGraph::CanCopyNodes() const
{
	
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		// If any of the nodes can be duplicated then we should allow copying
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEd->GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
		{
			UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
			if ((Node != NULL) && Node->CanDuplicateNode())
			{
				return true;
			}
		}
	}
	return false;

}

void FAssetEditor_StoryGraph::PasteNodes()
{
	
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		PasteNodesHere(FocusedGraphEd->GetPasteLocation());
	}
}

void FAssetEditor_StoryGraph::PasteNodesHere(const FVector2D& Location)
{
	
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		// Undo/Redo support
		const FScopedTransaction Transaction(FText::FromString("Custom Editor: Paste"));
		// TODO: Notify the data model of modification


		// Clear the selection set (newly pasted stuff will be selected)
		FocusedGraphEd->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		TSet<UEdGraphNode*> PastedNodes;

		FEdGraphUtilities::ImportNodesFromText(FocusedGraphEd->GetCurrentGraph(), TextToImport, /*out*/ PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		if (PastedNodes.Num() > 0)
		{
			float InvNumNodes = 1.0f / float(PastedNodes.Num());
			AvgNodePosition.X *= InvNumNodes;
			AvgNodePosition.Y *= InvNumNodes;
		}

		//Check nodes fron other graph and not user define

		const UEdGraphSchema_Base* GraphSchema = Cast<UEdGraphSchema_Base>(FocusedGraphEd->GetCurrentGraph()->GetSchema());
		
		
		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UProxyNodeBase* Node = (UProxyNodeBase*)*It;
			
			if (!Node->CanDuplicateNode() || (GraphSchema->SuitableDependetNodesType != UCustomNodeBase::GetIncertNodeType(Node->CustomNode->NodeType) && GraphSchema->SuitableStandaloneNodesType != UCustomNodeBase::GetIncertNodeType(Node->CustomNode->NodeType)))
			{
				
				FocusedGraphEd->GetCurrentGraph()->RemoveNode(Node);
				continue;
			}
			
			

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(SNodePanel::GetSnapGridSize());

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();

			// Duplicate custom node
			UCustomNodeBase* NewCustomNode = DuplicateObject<UCustomNodeBase>(Node->CustomNode, Node->CustomNode->GetOuter());
			
			if (UStoryGraph* StoryGraph = Cast<UStoryGraph>(((UEdGraph_StoryGraph*)Node->GetGraph())->GraphOwner))
			{
				StoryGraph->GarphNods.Add(NewCustomNode);
			}
			else if (UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(((UEdGraph_StoryGraph*)Node->GetGraph())->GraphOwner))
			{
				Charecter->GarphNods.Add(NewCustomNode);
			}
			else if (UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(((UEdGraph_StoryGraph*)Node->GetGraph())->GraphOwner))
			{
				PlaceTrigger->GarphNods.Add(NewCustomNode);
			}
			Node->CustomNode = NewCustomNode;

			// Select the newly pasted stuff
			FocusedGraphEd->SetNodeSelection(Node, true);
		}
		
		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UProxyNodeBase* Node = (UProxyNodeBase*)*It;
			UDialogNodeBase* DialogNodeBase = Cast<UDialogNodeBase>(Node->CustomNode);
			UStoryVerticalNodeBase* StoryVerticalNodeBase = Cast<UStoryVerticalNodeBase>(Node->CustomNode);
			TArray<UCustomNodeBase*> InputNodes;

			if (DialogNodeBase)
			{
				DialogNodeBase->GetInputNodes(InputNodes, EPinDataTypes::PinType_Horizontal);
				if (InputNodes.Num() == 0)
				{
					DialogNodeBase->RefreshDialogOwner();
				}
			}
			else if (StoryVerticalNodeBase)
			{
				StoryVerticalNodeBase->GetInputNodes(InputNodes, EPinDataTypes::PinType_Vertical);
				if (InputNodes.Num() == 0)
				{
					StoryVerticalNodeBase->RefreshQuestOwner();
				}
			}
		}
		// Update UI
		FocusedGraphEd->NotifyGraphChanged();
		EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
	}
}

bool  FAssetEditor_StoryGraph::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(AssetObject->FindGraph(EditedObject), ClipboardContent);
}

void FAssetEditor_StoryGraph::CutSelectedNodes()
{
	CopySelectedNodes();
	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_StoryGraph::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_StoryGraph::DuplicateNodes()
{
	// Copy and paste current selection
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_StoryGraph::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void  FAssetEditor_StoryGraph::DeleteSelectedDuplicatableNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();

	if (FocusedGraphEd.IsValid())
	{
		// Cache off the old selection
		const FGraphPanelSelectionSet OldSelectedNodes = FocusedGraphEd->GetSelectedNodes();

		// Clear the selection and only select the nodes that can be duplicated
		FGraphPanelSelectionSet RemainingNodes;
		FocusedGraphEd->ClearSelectionSet();

		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
		{
			UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
			if ((Node != NULL) && Node->CanDuplicateNode())
			{
				FocusedGraphEd->SetNodeSelection(Node, true);
			}
			else
			{
				RemainingNodes.Add(Node);
			}
		}

		// Delete the duplicatable nodes
		DeleteSelectedNodes();

		// Reselect whatever is left from the original selection after the deletion
		FocusedGraphEd->ClearSelectionSet();

		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
		{
			if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
			{
				FocusedGraphEd->SetNodeSelection(Node, true);
			}
		}
	}
}


TSharedRef<SGraphActionMenu> FAssetEditor_StoryGraph::CreateActionMenuWidget()
{


	// create the main action list piece of this widget
	TSharedRef<SGraphActionMenu> _GraphActionMenu = SNew(SGraphActionMenu, false)
		.OnGetFilterText(this, &FAssetEditor_StoryGraph::GetFilterText)
		.OnCreateWidgetForAction(this, &FAssetEditor_StoryGraph::OnCreateWidgetForAction)
		.OnCollectAllActions(this, &FAssetEditor_StoryGraph::CollectAllActions)
		.OnCollectStaticSections(this, &FAssetEditor_StoryGraph::CollectStaticSections)
		.OnActionDragged(this, &FAssetEditor_StoryGraph::OnActionDragged)
		//.OnCategoryDragged(this, &FAssetEditor_StoryGraph::OnCategoryDragged)
		.OnActionSelected(this, &FAssetEditor_StoryGraph::OnActionSelected)
		.OnActionDoubleClicked(this, &FAssetEditor_StoryGraph::OnActionDoubleClicked)
		.OnContextMenuOpening(this, &FAssetEditor_StoryGraph::OnContextMenuOpening)
		//.OnCategoryTextCommitted(this, &FAssetEditor_StoryGraph::OnCategoryNameCommitted)
		.OnCanRenameSelectedAction(this, &FAssetEditor_StoryGraph::CanRequestRenameOnActionNode)
		.OnGetSectionTitle(this, &FAssetEditor_StoryGraph::OnGetSectionTitle)
		.OnGetSectionToolTip(this, &FAssetEditor_StoryGraph::OnGetSectionToolTip)
		.OnGetSectionWidget(this, &FAssetEditor_StoryGraph::OnGetSectionWidget)
		.AlphaSortItems(false)
		.UseSectionStyling(true);
	
	
	return _GraphActionMenu;
}

//ActionMenuFunktions............................................................................................

FText FAssetEditor_StoryGraph::GetFilterText() const
{
	return FilterBox->GetText();
}

void FAssetEditor_StoryGraph::OnFilterTextChanged(const FText& InFilterText)
{
	GraphActionMenu->GenerateFilteredItems(false);
}

TSharedRef<SWidget> FAssetEditor_StoryGraph::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	
	TSharedRef<SGraphPaletteItem_StoryGraph> ActionWiget = SNew(SGraphPaletteItem_StoryGraph, InCreateData);
	ActionWiget->OnNameTextCommittedDelegate.BindSP(this, &FAssetEditor_StoryGraph::ActionNameCommitted);
	return  ActionWiget;
}

void FAssetEditor_StoryGraph::ActionNameCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	CurrentSelectedActionMenuObject->ObjName = NewText;
	ObjectNameChanged(CurrentSelectedActionMenuObject);
}

void FAssetEditor_StoryGraph::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	for (int i = 0; i < EditedObject->GarphObjects.Num(); i++)
	{
		
		TSharedPtr<FEdGraphSchemaAction_StoryGraph> NewVarAction = MakeShareable(new FEdGraphSchemaAction_StoryGraph(FText::FromString(EditedObject->GarphObjects[i]->Category),
			EditedObject->GarphObjects[i]->ObjName,
			FText::FromString("PropertyTooltip"),
			0,
			(int)EditedObject->GarphObjects[i]->ObjectType));
		
		NewVarAction->StoryGraphObjectNum = i;
		OutAllActions.AddAction(NewVarAction);
	}
}

void FAssetEditor_StoryGraph::CollectStaticSections(TArray<int32>& StaticSectionIDs)
{

	int i = 1;//Disabel EStoryObjectType::Non

	while (i < GetNumberEnums("EStoryObjectType"))
	{
		StaticSectionIDs.Add(i++);
	}
}

FReply FAssetEditor_StoryGraph::OnActionDragged(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent)
{
	if (!GraphEditor.IsValid())
	{
		return FReply::Unhandled();
	}
	if (InActions.Num() == 1)
	{
		TSharedRef<FDragDropAction_StoryGraph> DragOperation = FDragDropAction_StoryGraph::New(EditedObject->GarphObjects[((FEdGraphSchemaAction_StoryGraph*)InActions[0].Get())->StoryGraphObjectNum]);

		return FReply::Handled().BeginDragDrop(DragOperation);
	}
	
	return FReply::Unhandled();
}

//FReply FAssetEditor_StoryGraph::OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent)
//{
//	UE_LOG(StoryGraphEditor, Warning, TEXT("Drag category"));
//	TSharedRef<FMyBlueprintCategoryDragDropAction> DragOperation = FMyBlueprintCategoryDragDropAction::New(InCategory, SharedThis(this));
//	return FReply::Handled().BeginDragDrop(DragOperation);
//	//return FReply::Unhandled();
//}

void FAssetEditor_StoryGraph::OnActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType)
{
	if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || InSelectionType == ESelectInfo::OnNavigation || InActions.Num() == 0)
	{
		if (InActions.Num() != 0)
		{
			int Num = ((FEdGraphSchemaAction_StoryGraph*)InActions[0].Get())->StoryGraphObjectNum;
			CurrentSelectedActionMenuObject = EditedObject->GarphObjects[Num];
			CurrentSelectedObject = EditedObject->GarphObjects[Num];
			PropertyEditor->SetObject(EditedObject->GarphObjects[Num]);
		}
	}
}

void FAssetEditor_StoryGraph::OnActionDoubleClicked(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions)
{
	if (!GraphEditor.IsValid())
	{
		return;
	}
	int Num = ((FEdGraphSchemaAction_StoryGraph*)InActions[0].Get())->StoryGraphObjectNum;
	EditedObject->GarphObjects[Num]->DoubleClick();
	
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Action DoubelClicked"));
}


TSharedPtr<SWidget> FAssetEditor_StoryGraph::OnContextMenuOpening()
{
	if (!GraphEditor.IsValid())
	{
		return TSharedPtr<SWidget>();
	}
	
	FMenuBuilder MenuBuilder(true, GetToolkitCommands());
	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename);
	
	
	return MenuBuilder.MakeWidget();
}

//void FAssetEditor_StoryGraph::OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< FGraphActionNode > InAction)
//{
//	
//}



FText FAssetEditor_StoryGraph::OnGetSectionTitle(int32 InSectionID)
{
	
	return FText::FromString(UStoryGraphObject::GetObjectTypeEnumAsString((EStoryObjectType)InSectionID) + (FString("s")));
}

TSharedPtr<IToolTip> FAssetEditor_StoryGraph::OnGetSectionToolTip(int32 InSectionID)
{
	return SNew(SToolTip)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolTip.BrightBackground"))
		.Text(FText::FromString(UStoryGraphObject::GetObjectToolTip((EStoryObjectType)InSectionID)));
	
}

TSharedRef<SWidget> FAssetEditor_StoryGraph::OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID)
{
	TWeakPtr<SWidget> WeakRowWidget = RowWidget;

	FText AddNewText = FText::FromString(UStoryGraphObject::GetObjectTypeEnumAsString((EStoryObjectType)InSectionID));
	FName MetaDataTag = FName(*(FString("Add New ") + UStoryGraphObject::GetObjectTypeEnumAsString((EStoryObjectType)InSectionID)));

	return CreateAddToSectionButton(InSectionID, WeakRowWidget, AddNewText, MetaDataTag);
}

TSharedRef<SWidget> FAssetEditor_StoryGraph::CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "RoundButton")
		.ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
		.ContentPadding(FMargin(2, 0))
		.OnClicked(this, &FAssetEditor_StoryGraph::OnAddButtonClickedOnSection, InSectionID)
		//.IsEnabled(this, &SMyBlueprint::IsEditingMode)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AddMetaData<FTagMetaData>(FTagMetaData(MetaDataTag))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0, 1))
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("Plus"))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(2, 0, 0, 0))
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
				.Text(AddNewText)
				.Visibility(EVisibility::Visible)
				.ShadowOffset(FVector2D(1, 1))
			]
		];
}

FReply FAssetEditor_StoryGraph::OnAddButtonClickedOnSection(int32 InSectionID)
{

	AddStoryGraphObject(UStoryGraphObject::GetClassFromStoryObjectType((EStoryObjectType)InSectionID));
	return FReply::Handled();
}

void FAssetEditor_StoryGraph::AddStoryGraphObject(TSubclassOf<UStoryGraphObject> Class)
{
	UStoryGraphObject* StoryGraphObject = NewObject<UStoryGraphObject>(EditedObject, Class);

	EditedObject->GarphObjects.Add(StoryGraphObject);
	GraphActionMenu->RefreshAllActions(true);
	
	EditedObject->Modify(); //Set graph modify
	EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
}

void FAssetEditor_StoryGraph::DeleteStoryGraphObject()
{

	TArray<UEdGraphNode*> NodesToDelete;
	
	if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Delet object " + CurrentSelectedActionMenuObject->ObjName.ToString() + "?")) == EAppReturnType::Ok)
	{

		FindDependetNodsInGraph(NodesToDelete, CurrentSelectedActionMenuObject);
		
		DeleteNodes(NodesToDelete);

		UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(CurrentSelectedActionMenuObject);
		UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentSelectedActionMenuObject);

		if (Charecter)
		{
			UEdGraph_StoryGraph* GraphToDelet = AssetObject->FindGraph(Charecter);
			if (GraphToDelet)
			{
				AssetObject->RemoveGraph(GraphToDelet);
			}
		}
		else if (PlaceTrigger)
		{
			UEdGraph_StoryGraph* GraphToDelet = AssetObject->FindGraph(PlaceTrigger);
			if (GraphToDelet)
			{
				AssetObject->RemoveGraph(GraphToDelet);
			}
		}
		
		EditedObject->GarphObjects.RemoveSingle(CurrentSelectedActionMenuObject); //Delet object

		GraphActionMenu->RefreshAllActions(true);

		EditedObject->Modify(); //Set graph modify
		EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
	}
}

void FAssetEditor_StoryGraph::FindDependetNodsInGraph(TArray<UEdGraphNode*>& Nodes, UStoryGraphObject* OwningObject)
{
	Nodes.Empty();

	for (int j = 0; j < AssetObject->Graphs.Num(); j++)
	{
		for (int i = 0; i < AssetObject->Graphs[j]->Nodes.Num(); i++)
		{

			if (UProxyNodeBase* CurrentNode = Cast<UProxyNodeBase>(AssetObject->Graphs[j]->Nodes[i]))
			{
				if (UCustomNodeBase::GetIncertNodeType(CurrentNode->CustomNode->NodeType) == EIncertNodeType::StoryGraphDependent ||
					UCustomNodeBase::GetIncertNodeType(CurrentNode->CustomNode->NodeType) == EIncertNodeType::MessageGraphDependent ||
					UCustomNodeBase::GetIncertNodeType(CurrentNode->CustomNode->NodeType) == EIncertNodeType::DialogGraphDependent)
				{
					if (CurrentNode->CustomNode->pGraphObject == OwningObject)
					{
						Nodes.Add(CurrentNode);
					}
				}
			}

		}
	}

}

void FAssetEditor_StoryGraph::CompilStoryObjects()
{
	
	bool CheckResult = true;

	for (int j = 0; j < EditedObject->GarphObjects.Num(); j++)
	{
		if (UStoryGraphObjectWithScenObject* GraphObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(EditedObject->GarphObjects[j]))
		{
			if (GraphObjectWithScenObject->GetScenObjectsNum() == 0)
			{
				UStoryGraphInventoryItem* InventoryItem = Cast<UStoryGraphInventoryItem>(GraphObjectWithScenObject);
				
				if (InventoryItem && InventoryItem->InventoryItemWithoutScenObject)
				{
					continue;
				}
				CheckResult = false;
				UE_LOG(LogCategoryStoryGraphPluginEditor, Warning, TEXT("%s %s dont have any reference"), *UStoryGraphObject::GetObjectTypeEnumAsString(EditedObject->GarphObjects[j]->ObjectType),
					*EditedObject->GarphObjects[j]->ObjName.ToString());
				
			}
		}
		
	}
	if (CheckResult)
	{
		EditedObject->StoryGraphState = EStoryGraphState::ST_Compile;
	}
	else
	{
		EditedObject->StoryGraphState = EStoryGraphState::ST_Error;
	}

	EditedObject->CompilationCounter++;
	EditedObject->LoadedCompilationCounter = EditedObject->CompilationCounter;
	for (TActorIterator<AStoryGraphActor> ActorItr(GWorld); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass()->ClassGeneratedBy == AssetObject)
		{
			ActorItr->Modify();
			break;
		}
	}
	
}

void FAssetEditor_StoryGraph::ExportInXMLFile()
{
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	const void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;
	if (!ParentWindowHandle) return;

	TArray<FString> OutputFile;
	if (FDesktopPlatformModule::Get()->SaveFileDialog(ParentWindowHandle, FString("Save XML File"), FPaths::ProjectDir(), FString("XMLFile.xml"), FString("XML|*.xml"), EFileDialogFlags::Multiple, OutputFile))
	{
		const FString FileTemplate = "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n<root>\n</root>";
		FXmlFile File(FileTemplate, EConstructMethod::ConstructFromBuffer);

		FXmlNode* RootNode = File.GetRootNode();

		RootNode->AppendChildNode("GraphObjects", "");
		FXmlNode* GraphObjectsNode = RootNode->FindChildNode("GraphObjects");
		
		SerializedStoryGraphObjectsToXMLNode(GraphObjectsNode);


		RootNode->AppendChildNode("DialogGraphs", "");
		FXmlNode* DiaogGraphsNode = RootNode->FindChildNode("DialogGraphs");

		for (int i = 0; i < EditedObject->GarphObjects.Num(); i++)
		{
			if (UStoryGraphCharecter* pStoryGraphCharecter = Cast<UStoryGraphCharecter>(EditedObject->GarphObjects[i]))
			{
				for (int j = 0; j < pStoryGraphCharecter->GarphNods.Num(); j++)
				{
					pStoryGraphCharecter->GarphNods[j]->XMLID = EditedObject->GarphObjects[i]->XMLID + "_" + FString::FromInt(j);
				}

				DiaogGraphsNode->AppendChildNode(EditedObject->GarphObjects[i]->XMLID, "");
				FXmlNode* NewDialogNode = DiaogGraphsNode->FindChildNode(EditedObject->GarphObjects[i]->XMLID);

				SerializedGraphNodesToXMLNode(NewDialogNode, EditedObject->GarphObjects[i]);
				
			}
			else if (UStoryGraphPlaceTrigger* pStoryGraphPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(EditedObject->GarphObjects[i]))
			{
				for (int j = 0; j < pStoryGraphPlaceTrigger->GarphNods.Num(); j++)
				{
					pStoryGraphPlaceTrigger->GarphNods[j]->XMLID = EditedObject->GarphObjects[i]->XMLID + "_" + FString::FromInt(j);;
				}

				DiaogGraphsNode->AppendChildNode(EditedObject->GarphObjects[i]->XMLID, "");
				FXmlNode* NewDialogNode = DiaogGraphsNode->FindChildNode(EditedObject->GarphObjects[i]->XMLID);

				SerializedGraphNodesToXMLNode(NewDialogNode, EditedObject->GarphObjects[i]);

			}
		}

		for (int i = 0; i < EditedObject->GarphNods.Num(); i++)
		{
			EditedObject->GarphNods[i]->XMLID = "M_" + FString::FromInt(i);
		}

		RootNode->AppendChildNode("MainGraph", "");
		FXmlNode* MainGraphNode = RootNode->FindChildNode("MainGraph");

		SerializedGraphNodesToXMLNode(MainGraphNode, EditedObject);

		File.Save(OutputFile[0]);
	}
	
}

void FAssetEditor_StoryGraph::SerializedStoryGraphObjectsToXMLNode(FXmlNode* XMLNode)
{
	for (int i = 0; i < EditedObject->GarphObjects.Num(); i++)
	{

		EditedObject->GarphObjects[i]->XMLID = FString::FromInt(i);
		XMLNode->AppendChildNode(EditedObject->GarphObjects[i]->XMLID, "");
		FXmlNode* ObjectNode = XMLNode->FindChildNode(EditedObject->GarphObjects[i]->XMLID);

		std::map<FString, XMLProperty> Propertys;
		EditedObject->GarphObjects[i]->GetXMLSavingProperty(Propertys);

		for (auto it = Propertys.begin(); it != Propertys.end(); ++it)
		{
			if (it->second.Propertys.size() != 0)
			{
				CreateXMLArray(it->second, it->first, ObjectNode);

			}
			else
			{
				ObjectNode->AppendChildNode(it->first, it->second.Val);
			}
		}

	}
}

void FAssetEditor_StoryGraph::SerializedGraphNodesToXMLNode(FXmlNode* XMLNode, UObject* OwnedGraphObject)
{
	UEdGraph_StoryGraph* Graph = AssetObject->FindGraph(OwnedGraphObject);
	if (!Graph)
	{
		return;
	}
	TArray<UProxyNodeBase*> GraphNods;
	Graph->GetNodesOfClass<UProxyNodeBase>(GraphNods);

	for (int i = 0; i < GraphNods.Num(); i++)
	{
		XMLNode->AppendChildNode(GraphNods[i]->CustomNode->XMLID, "");
		FXmlNode* NewGraphNode = XMLNode->FindChildNode(GraphNods[i]->CustomNode->XMLID);

		NewGraphNode->AppendChildNode("X", FString::FromInt(GraphNods[i]->NodePosX));
		NewGraphNode->AppendChildNode("Y", FString::FromInt(GraphNods[i]->NodePosY));

		std::map<FString, XMLProperty> Propertys;
		GraphNods[i]->CustomNode->GetXMLSavingProperty(Propertys);

		for (auto it = Propertys.begin(); it != Propertys.end(); ++it)
		{
			if (it->second.Propertys.size() != 0)
			{
				CreateXMLArray(it->second, it->first, NewGraphNode);

			}
			else
			{
				NewGraphNode->AppendChildNode(it->first, it->second.Val);
			}
		}
	}
}



void FAssetEditor_StoryGraph::CreateXMLArray(XMLProperty& Property, FString Name, FXmlNode* RootNode)
{
	
	RootNode->AppendChildNode(Name, Property.Val);
	FXmlNode* ArrayRootNode = RootNode->FindChildNode(Name);

	for (auto it = Property.Propertys.begin(); it != Property.Propertys.end(); ++it)
	{
		
		if (it->second.Propertys.size() != 0)
		{
			CreateXMLArray(it->second, it->first, ArrayRootNode);
		}
		else
		{
			ArrayRootNode->AppendChildNode(it->first, it->second.Val);
		}
	}

}


void FAssetEditor_StoryGraph::ImportFromXMLFile()
{
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	const void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;
	if (!ParentWindowHandle) return;

	TArray<FString> OutputFile;
	if (FDesktopPlatformModule::Get()->OpenFileDialog(ParentWindowHandle, FString("Open XML File"), FPaths::ProjectDir(), FString(""), FString("XML|*.xml"), EFileDialogFlags::Multiple, OutputFile))
	{
		EraseStroyGraph(); 

		FXmlFile File(OutputFile[0], EConstructMethod::ConstructFromFile);
		FXmlNode* RootNode = File.GetRootNode();

		if (!RootNode)
		{
			UE_LOG(LogCategoryStoryGraphPluginEditor, Error, TEXT("Bad file"));
			return;
		}

		FXmlNode* GraphObjectsNode = RootNode->FindChildNode("GraphObjects");
		SpawnStoryGraphObjectsFromXMLNode(GraphObjectsNode);

		FXmlNode* DialogGraphs = RootNode->FindChildNode("DialogGraphs");
		TArray<FXmlNode*> CurrentDialogGraph = DialogGraphs->GetChildrenNodes();

		for (int i = 0; i < CurrentDialogGraph.Num(); i++)
		{
			UEdGraph_StoryGraph* NewDialogGraph = CreateDialogGraph(EditedObject->GarphObjects[FCString::Atoi(*CurrentDialogGraph[i]->GetTag())]);

			SpawnGraphNodsFromXMLNode(CurrentDialogGraph[i], EditedObject->GarphObjects[FCString::Atoi(*CurrentDialogGraph[i]->GetTag())]);

		}
		
		FXmlNode* MainGraph = RootNode->FindChildNode("MainGraph");
		SpawnGraphNodsFromXMLNode(MainGraph, EditedObject);
		

		GraphActionMenu->RefreshAllActions(true);
		EditedObject->Modify(); //Set graph modify
		EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
	}
	
	
}

void FAssetEditor_StoryGraph::SpawnStoryGraphObjectsFromXMLNode(FXmlNode* GraphObjectsNode)
{
	TArray<FXmlNode*> CurrentObjectNode = GraphObjectsNode->GetChildrenNodes();
	for (int i = 0; i < CurrentObjectNode.Num(); i++)
	{

		EStoryObjectType ObjectType = GetEnumValueFromString<EStoryObjectType>("EStoryObjectType", CurrentObjectNode[i]->FindChildNode("ObjectType")->GetContent());

		if (ObjectType != EStoryObjectType::Non)
		{
			UStoryGraphObject* StoryGraphObject = NewObject<UStoryGraphObject>(EditedObject, UStoryGraphObject::GetClassFromStoryObjectType(ObjectType));
			EditedObject->GarphObjects.Add(StoryGraphObject);

			std::map<FString, XMLProperty> Propertys;
			FillPropertyMapFromXMLNode(Propertys, CurrentObjectNode[i]);
			StoryGraphObject->LoadPropertyFromXML(Propertys);
		}
	}
}

void FAssetEditor_StoryGraph::SpawnGraphNodsFromXMLNode(FXmlNode* XMLNode, UObject* OwnedGraphObject)
{
	TArray<FXmlNode*> XMLGraphNods = XMLNode->GetChildrenNodes();
	UEdGraph_StoryGraph* ParentGraph = AssetObject->FindGraph(OwnedGraphObject);

	if (!ParentGraph) return;

	for (int i = 0; i < XMLGraphNods.Num(); i++)
	{
		ENodeType NodeType = GetEnumValueFromString<ENodeType>("ENodeType", XMLGraphNods[i]->FindChildNode("NodeType")->GetContent());

		UStoryGraphObject* OwnedObject = NULL;
		if (XMLGraphNods[i]->FindChildNode("GraphOject")->GetContent() != "Non")
		{
			OwnedObject = EditedObject->GarphObjects[FCString::Atoi(*XMLGraphNods[i]->FindChildNode("GraphOject")->GetContent())];
		}

		FVector2D NodeLocation(FCString::Atoi(*XMLGraphNods[i]->FindChildNode("X")->GetContent()), FCString::Atoi(*XMLGraphNods[i]->FindChildNode("Y")->GetContent()));

		UProxyNodeBase* NewNode = (UProxyNodeBase*)FCustomSchemaAction_NewNode::SpawnNode(NodeType, OwnedObject, ParentGraph, NULL, NodeLocation, false);
		std::map<FString, XMLProperty> Propertys;
		FillPropertyMapFromXMLNode(Propertys, XMLGraphNods[i]);
		NewNode->CustomNode->LoadPropertyFromXML(Propertys);
	}

	
	TArray<UProxyNodeBase*> GraphNods;
	ParentGraph->GetNodesOfClass<UProxyNodeBase>(GraphNods);

	for (int i = 0; i < GraphNods.Num(); i++)
	{
		TArray<FXmlNode*> XMLPins = XMLGraphNods[i]->FindChildNode("Arr_Pins")->GetChildrenNodes();
		for (int j = 0; j < XMLPins.Num(); j++)
		{
			TArray<FXmlNode*> XMLLinks = XMLPins[j]->GetChildrenNodes();
			for (int k = 0; k < XMLLinks.Num(); k++)
			{
				int32 SignNum = XMLLinks[k]->GetContent().Find("_");
					
				ParentGraph->GetSchema()->TryCreateConnection(GraphNods[i]->Pins[j], GraphNods[FCString::Atoi(*XMLLinks[k]->GetContent().RightChop(SignNum+1))]->Pins[0]);
			}
				
		}
	}
	
}

void FAssetEditor_StoryGraph::FillPropertyMapFromXMLNode(std::map<FString, XMLProperty>& Propertys, FXmlNode* CurrentObjectNode)
{
	TArray<FXmlNode*> PropertyNodes = CurrentObjectNode->GetChildrenNodes();

	for (int j = 0; j < PropertyNodes.Num(); j++)
	{


		if (PropertyNodes[j]->GetTag().Find("Arr") != -1)
		{
			int32 SignPoz = PropertyNodes[j]->GetTag().Find("_");
			Propertys.insert(std::pair<FString, XMLProperty>(PropertyNodes[j]->GetTag(), XMLProperty(PropertyNodes[j]->GetContent())));

			CreatePropertyArray(Propertys[PropertyNodes[j]->GetTag()], PropertyNodes[j]);
		}
		else
		{
			Propertys.insert(std::pair<FString, XMLProperty>(PropertyNodes[j]->GetTag(), XMLProperty(PropertyNodes[j]->GetContent())));
		}
	}
}

void FAssetEditor_StoryGraph::CreatePropertyArray(XMLProperty& Property, FXmlNode* RootNode)
{
	TArray<FXmlNode*> ArrayNodes = RootNode->GetChildrenNodes();

	for (int i = 0; i < ArrayNodes.Num(); i++)
	{
		if (ArrayNodes[i]->GetTag().Find("Arr") != -1)
		{
			Property.Propertys.insert(std::pair<FString, XMLProperty>(ArrayNodes[i]->GetTag(), XMLProperty(ArrayNodes[i]->GetContent())));
			CreatePropertyArray(Property.Propertys[ArrayNodes[i]->GetTag()], ArrayNodes[i]);
		}
		else
		{
			Property.Propertys.insert(std::pair<FString, XMLProperty>(ArrayNodes[i]->GetTag(), XMLProperty(ArrayNodes[i]->GetContent())));
		}
	}
}

void FAssetEditor_StoryGraph::UnlinkAllStoryGraphObjects()
{
	if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Remove all links to scen object from all StoryGraph objects?")) == EAppReturnType::Ok)
	{
		for (int i = 0; i < EditedObject->GarphObjects.Num(); i++)
		{
			UStoryGraphObjectWithScenObject* StoryGraphObjectWithScenObject = Cast<UStoryGraphObjectWithScenObject>(EditedObject->GarphObjects[i]);
			if (StoryGraphObjectWithScenObject)
			{
				StoryGraphObjectWithScenObject->ClearScenObjects();
			}
		}
	}
}

void FAssetEditor_StoryGraph::OnMyRequestRenameOnActionNode()
{
	// Attempt to rename in both menus, only one of them will have anything selected
	GraphActionMenu->OnRequestRenameOnActionNode();
}

bool FAssetEditor_StoryGraph::CanRequestRenameOnActionNode() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	// If there is anything selected in the GraphActionMenu, check the item for if it can be renamed.
	if (SelectedActions.Num())
	{
		return GraphActionMenu->CanRequestRenameOnActionNode();
	}
	return false;
}

bool FAssetEditor_StoryGraph::CanRequestRenameOnActionNode(TWeakPtr<struct FGraphActionNode> InSelectedNode) const
{
	return true;
}


void FAssetEditor_StoryGraph::OpenDialogEditorTab(UStoryGraphObject* pStoryGraphObject)
{
	UStoryGraphCharecter* pStoryGraphCharecter = Cast<UStoryGraphCharecter>(pStoryGraphObject);
	UStoryGraphPlaceTrigger* pStoryGraphPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(pStoryGraphObject);
	
	UEdGraph_StoryGraph* DialogGraph = CreateDialogGraph(pStoryGraphObject);

	int TabNum = -1;

	for (int i = 0; i < DialogTabs.Num(); i++)
	{
		SGraphEditor& rGraphEditor = (SGraphEditor&)DialogTabs[i]->GetContent().Get();
		if (rGraphEditor.GetCurrentGraph() == DialogGraph)
		{
			TabNum = i;
			break;
		}
	}

	TSharedPtr<SDockTab>DialogTabLoc;

	if (TabNum == -1)
	{
		if (pStoryGraphCharecter)
		{
			TSharedPtr<SGraphEditor> DialogEditor = CreateGraphEditorWidget(DialogGraph, "Dialog Graph");
			DialogTabLoc = CreateDialogTab(DialogEditor, "DG_" + pStoryGraphObject->ObjName.ToString());
		}
		else if (pStoryGraphPlaceTrigger)
		{
			TSharedPtr<SGraphEditor> DialogEditor = CreateGraphEditorWidget(DialogGraph, "Message Graph");
			DialogTabLoc = CreateDialogTab(DialogEditor, "MS_" + pStoryGraphObject->ObjName.ToString());
		}
		
		DialogTabs.Add(DialogTabLoc);
	}
	else
	{
		DialogTabLoc = DialogTabs[TabNum];
		DialogTabLoc->RequestCloseTab();
	}

	TabManager->InsertNewDocumentTab(FCustomEditorTabs::ViewportID, FTabManager::ESearchPreference::RequireClosedTab, DialogTabLoc.ToSharedRef());
	TabManager->DrawAttention(DialogTabLoc.ToSharedRef());
	
}

UEdGraph_StoryGraph* FAssetEditor_StoryGraph::CreateDialogGraph(UStoryGraphObject* pStoryGraphObject)
{
	UStoryGraphCharecter* pStoryGraphCharecter = Cast<UStoryGraphCharecter>(pStoryGraphObject);
	UStoryGraphPlaceTrigger* pStoryGraphPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(pStoryGraphObject);

	UEdGraph_StoryGraph* DialogGraph = AssetObject->FindGraph(pStoryGraphObject);

	if (!DialogGraph)
	{
		DialogGraph = NewObject<UEdGraph_StoryGraph>(AssetObject, UEdGraph_StoryGraph::StaticClass(), NAME_None, RF_Transactional);
		AssetObject->Graphs.Add(DialogGraph);
		if (pStoryGraphCharecter)
		{
			DialogGraph->Schema = UEdGraphSchema_DialogGraph::StaticClass();
		}
		else if (pStoryGraphPlaceTrigger)
		{
			DialogGraph->Schema = UEdGraphSchema_MessageGraph::StaticClass();
		}
		DialogGraph->StoryGraph = EditedObject;
		DialogGraph->GraphOwner = pStoryGraphObject;
	}

	return DialogGraph;
}

TSharedRef<SDockTab> FAssetEditor_StoryGraph::CreateDialogTab(const TSharedPtr<SGraphEditor> DialogEditor,  FString TabName)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TabName))
		.TabRole(ETabRole::DocumentTab)
		.TabColorScale(GetTabColorScale())
		[
			DialogEditor.ToSharedRef()
		];

}


void FAssetEditor_StoryGraph::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	// Update the graph editor that is currently focused
	FocusedGraphEdPtr = InGraphEditor;


	

	// During undo, garbage graphs can be temporarily brought into focus, ensure that before a refresh of the MyBlueprint window that the graph is owned by a Blueprint
	if (FocusedGraphEdPtr.IsValid())
	{
		
		// The focused graph can be garbage as well
		TWeakObjectPtr< UEdGraph > FocusedGraphPtr = FocusedGraphEdPtr.Pin()->GetCurrentGraph();
		UEdGraph* FocusedGraph = FocusedGraphPtr.Get();
		
		

	}
}

void FAssetEditor_StoryGraph::ShowNotification(FString Text, SNotificationItem::ECompletionState State)
{
	UE_LOG(LogCategoryStoryGraphPluginEditor, Error, TEXT("Notification"));
	FNotificationInfo Info(FText::FromString(Text));
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 1.0f;
	Info.ExpireDuration = 2.0f;

	TWeakPtr<SNotificationItem> NotificationPtr = FSlateNotificationManager::Get().AddNotification(Info);
	if (NotificationPtr.IsValid())
	{
		NotificationPtr.Pin()->SetCompletionState(State);
	}
}

FSlateIcon FAssetEditor_StoryGraph::GetStatusImage() const
{
	
	switch (EditedObject->StoryGraphState)
	{
	case EStoryGraphState::ST_Compile:

		return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Good");

	case EStoryGraphState::ST_Error:

		return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Warning");

	case EStoryGraphState::ST_Modify:

		return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Unknown");
	}

	return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Good");
}

void FAssetEditor_StoryGraph::JumpToNode(const UEdGraphNode* Node, bool bRequestRename)
{
	if (GraphEditor.IsValid())
	{
		GraphEditor->JumpToNode(Node, bRequestRename);
	}
}

void FAssetEditor_StoryGraph::RefreshDetailPanel()
{
	if (CurrentSelectedObject)
	{
		if (UProxyNodeBase* ProxyNode = Cast<UProxyNodeBase>(CurrentSelectedObject))
		{
			PropertyEditor->SetObject(ProxyNode->CustomNode, true);
		}
		else
		{
			PropertyEditor->SetObject(CurrentSelectedObject, true);
		}
	}
}

void FAssetEditor_StoryGraph::EraseStroyGraph()
{
	//Erase main graph

	UEdGraph_StoryGraph* MainGraph = AssetObject->FindGraph(EditedObject);

	TArray<UEdGraphNode*> NodesToDelete;
	MainGraph->GetNodesOfClass<UEdGraphNode>(NodesToDelete);

	DeleteNodes(NodesToDelete);

	//Erase dialog graphs

	for (int i = 0; i < EditedObject->GarphObjects.Num(); i++)
	{
		if (EditedObject->GarphObjects[i]->ObjectType == EStoryObjectType::Character || EditedObject->GarphObjects[i]->ObjectType == EStoryObjectType::PlaceTrigger)
		{
			UEdGraph_StoryGraph* DialogGraph = AssetObject->FindGraph(EditedObject);
			TArray<UEdGraphNode*> DialogNodesToDelete;
			DialogGraph->GetNodesOfClass<UEdGraphNode>(DialogNodesToDelete);
			DeleteNodes(DialogNodesToDelete);
		}
	}

	EditedObject->GarphObjects.Empty();

	EditedObject->Modify(); //Set graph modify
	EditedObject->StoryGraphState = EStoryGraphState::ST_Modify;
}