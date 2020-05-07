// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "IDetailCustomization.h"
#include "Runtime/SlateCore/Public/Input/Reply.h"
#include "Runtime/SlateCore/Public/Types/SlateEnums.h"
#include "Runtime/Slate/Public/Widgets/Input/SMenuAnchor.h"

class UDialogStartNode;

class FCustomNodeBaseDetail : public IDetailCustomization
{
public:
	//FStoryGraphObjectDetail() {}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static class UCustomNodeBase* GetDetailObject(IDetailLayoutBuilder* DetailBuilder);

	void CommentCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);

protected:
	class IDetailCategoryBuilder* pMainPanel;

	IDetailLayoutBuilder* pDetailBuilder;
};

class FDialogNodeBaseDetail : public FCustomNodeBaseDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:

	class IDetailPropertyRow* pDialogOwnerProperty;
};

class FDialogDependedNodeDetail : public FDialogNodeBaseDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FStoryGraphDependedNodeDetail : public FCustomNodeBaseDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};


class FDialogStartNodeDetail : public FDialogNodeBaseDetail
{
public:


	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:


	void DialogCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);
};

class FDialogNodeDetail : public FDialogNodeBaseDetail
{
public:


	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	static FReply AddDialogButtonClick(IDetailLayoutBuilder* DetailBuilder);

	static FReply RemoveDialogButtonClick(IDetailLayoutBuilder* DetailBuilder, int Num);

	void DialogCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder,
	                     int Num);
};

class FGetStoryGraphObjectStateNodeDetail : public FStoryGraphDependedNodeDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;


private:

	FReply HandleGraphObjectStateListButtonClicked();

	TSharedRef<SWidget> GetGraphObjectStateMenuOptions();

	void HandleSetCurrentGraphObjectState(int WantedState);

	TSharedPtr<SMenuAnchor> ObjectStatesListBox;

	TSharedPtr<STextBlock> WantedObjectStateTextBox;
};


class FAddDialogNodeDetail : public FStoryGraphDependedNodeDetail
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	TSharedPtr<SMenuAnchor> ObjectStatesListBox;

	TSharedPtr<STextBlock> SelectedDialogTextBox;

	FReply HandleAvailableDialogsButtonClicked();

	TSharedRef<SWidget> GetAvailableDialogs();

	void HandleSetCurrentDialog(UDialogStartNode* SelectedDialog);
};

class FAddDialogFromDialogDetail : public FDialogNodeBaseDetail
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	TSharedPtr<SMenuAnchor> ObjectStatesListBox;

	TSharedPtr<STextBlock> SelectedDialogTextBox;

	FReply HandleAvailableDialogsButtonClicked();

	TSharedRef<SWidget> GetAvailableDialogs();

	void HandleSetCurrentDialog(UDialogStartNode* SelectedDialog);
};

class FAddQuestPhaseNodeDetail : public FStoryGraphDependedNodeDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:


	void QuestPhaseCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);
};

class FQuestStarNodeDetail : public FStoryGraphDependedNodeDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FSetInventoryItemStateNodeDetail : public FStoryGraphDependedNodeDetail
{
private:

	TSharedPtr<SMenuAnchor> InventoryItemPhaseListBox;

	TSharedPtr<STextBlock> SelectedInventoryItemPhaseTextBox;

public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	FReply HandleAvailableDialogsButtonClicked();

	TSharedRef<SWidget> GetAvailableInventoryItemPhase();

	void HandleSetCurrentInventoryItemState(int PhaseNumber);
};
