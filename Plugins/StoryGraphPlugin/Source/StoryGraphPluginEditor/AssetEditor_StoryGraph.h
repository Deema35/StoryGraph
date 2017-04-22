// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "Toolkits/AssetEditorToolkit.h"
#include "SNotificationList.h"
#include "EngineMinimal.h"
#include <map>
#include "NotifyHook.h"


struct FCustomEditorTabs
{
	static const FName DetailsID;
	static const FName ViewportID;
	static const FName ActionMenuID;
};

class FAssetEditor_StoryGraph : public FAssetEditorToolkit, public FNotifyHook
{

protected:
	

	TSharedPtr<class SGraphActionMenu> GraphActionMenu;
	TSharedPtr<class SGraphEditor> GraphEditor;
	TSharedPtr<class IDetailsView> PropertyEditor;

	TArray<TSharedPtr<SDockTab>> DialogTabs;

	/** The filter box that handles filtering for both graph action menus. */
	TSharedPtr< class SSearchBox > FilterBox;
	// Factory that spawns graph editors; used to look up all tabs spawned by it.
	TWeakPtr<class FDocumentTabFactory> GraphEditorTabFactoryPtr;
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TSharedPtr<FExtender> ToolbarExtender;

	class UStoryGraphBlueprint* AssetObject;
	class UStoryGraph* EditedObject;
	

private:

	/** Currently focused graph editor */
	TWeakPtr<class SGraphEditor> FocusedGraphEdPtr;
	/** Handle to the registered OnTabForegrounded delegate */
	FDelegateHandle TabActiveDelegateHandle;
	FDelegateHandle TabForegroundedDelegateHandle;
	class UStoryGraphObject* CurrentSelectedActionMenuObject;
	class UObject* CurrentSelectedObject;

public:
	FAssetEditor_StoryGraph();
	~FAssetEditor_StoryGraph();
	
	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	
	
	// FAssetEditorToolkit
	virtual FText GetToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual void GetSaveableObjects(TArray<UObject*>& OutObjects) const override;

	void InitAssetEditor_StoryGraph(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit);
	void OpenDialogEditorTab(class UStoryGraphObject* pStoryGraphObject);
	
	void ClearSelection();
	void JumpToNode(const class UEdGraphNode* Node, bool bRequestRename = false);
	void RefreshDetailPanel();

private:
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(class UEdGraph* InGraph, FString CornerText);
	TSharedRef<class SGraphActionMenu> CreateActionMenuWidget();

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ActionMenu(const FSpawnTabArgs& Args);

	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete);
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;
	void DeleteSelectedDuplicatableNodes();
	
	/** Called when the selection changes in the GraphEditor */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	/** Called when a node is double clicked */
	//void OnNodeDoubleClicked(class UEdGraphNode* Node);
	void OnPropertiesChanging(const FPropertyChangedEvent& e);
	void ObjectNameChanged(class UStoryGraphObject* ChangObject);
	

	void AddToolbarExtension(FToolBarBuilder &builder);
	void BindCommands();

	//SearchTextActions
	void OnFilterTextChanged(const FText& InFilterText);


	//MenuActionsMetords
	FText GetFilterText() const;
	TSharedRef<SWidget> OnCreateWidgetForAction(struct FCreateWidgetForActionData* const InCreateData);
	void ActionNameCommitted(const FText& NewText, ETextCommit::Type InTextCommit);
	void CollectAllActions(struct FGraphActionListBuilderBase& OutAllActions);
	void CollectStaticSections(TArray<int32>& StaticSectionIDs);
	FReply OnActionDragged(const TArray< TSharedPtr<struct FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent);
	//FReply OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent);
	void OnActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType);
	void OnActionDoubleClicked(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions);
	TSharedPtr<SWidget> OnContextMenuOpening();
	//void OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< FGraphActionNode > InAction);
	bool CanRequestRenameOnActionNode(TWeakPtr<struct FGraphActionNode> InSelectedNode) const;
	FText OnGetSectionTitle(int32 InSectionID);
	TSharedPtr<IToolTip> OnGetSectionToolTip(int32 InSectionID);
	TSharedRef<SWidget> OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID);
	TSharedRef<SWidget> CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag);
	FReply OnAddButtonClickedOnSection(int32 InSectionID);

	

	void AddStoryGraphObject(TSubclassOf<class UStoryGraphObject>);
	TSharedRef<SDockTab> CreateDialogTab(const TSharedPtr<SGraphEditor> DialogEditor, FString TabName);
	void DeleteStoryGraphObject();
	void CompilStoryObjects();
	void ExportInXMLFile();
	void ImportFromXMLFile();
	void UnlinkAllStoryGraphObjects();
	void FindDependetNodsInGraph(TArray<class UEdGraphNode*>& Nodes, UStoryGraphObject* OwningObject);
	void OnMyRequestRenameOnActionNode();
	bool CanRequestRenameOnActionNode() const;
	void OnTabForegrounded(TSharedPtr<SDockTab> ForegroundedTab, TSharedPtr<SDockTab> BackgroundedTab);
	static void ShowNotification(FString Text, SNotificationItem::ECompletionState State = SNotificationItem::CS_None);
	FSlateIcon GetStatusImage() const;
	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);
	void SerializedStoryGraphObjectsToXMLNode(class FXmlNode* XMLNode);
	void SerializedGraphNodesToXMLNode(class FXmlNode* XMLNode, UObject* OwnedGraphObject);
	static void CreateXMLArray(struct XMLProperty& Property, FString Name, FXmlNode* RootNode);
	static void CreatePropertyArray(XMLProperty& Property, FXmlNode* PropertyNode);
	void SpawnStoryGraphObjectsFromXMLNode(FXmlNode* GraphObjectsNode);
	void SpawnGraphNodsFromXMLNode(FXmlNode* XMLNode, UObject* OwnedGraphObject);
	class UEdGraph_StoryGraph* CreateDialogGraph(UStoryGraphObject* pStoryGraphObject);
	void FillPropertyMapFromXMLNode(std::map<FString, XMLProperty>& Propertys, FXmlNode* CurrentObjectNode);
	void EraseStroyGraph();
};






