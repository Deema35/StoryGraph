// Copyright 2016 Dmitriy Pavlov


#include "DetailPanelCustomizationNodes.h"
#include "PropertyEditorModule.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "StoryGraphObject.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "GraphSchema_StoryGraph.h"
#include "StoryGraph.h"
#include "CustomNods.h"
#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"

//FCustomNodeBaseDetail.............................................................................................

TSharedRef<IDetailCustomization> FCustomNodeBaseDetail::MakeInstance()
{
	return MakeShareable(new FCustomNodeBaseDetail);
}



void FCustomNodeBaseDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	pDetailBuilder = &DetailBuilder;
	pMainPanel = &(DetailBuilder.EditCategory("Main", FText::GetEmpty(), ECategoryPriority::Important));
	IDetailCategoryBuilder& CommentPanel = DetailBuilder.EditCategory("Commentary", FText::GetEmpty(), ECategoryPriority::Uncommon);

	pMainPanel->AddCustomRow(FText::FromString("Main"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Node Name"))
		]
		.ValueContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(UCustomNodeBase::GetActionNameFromNodeType(GetDetailObject(&DetailBuilder)->NodeType)))
		];

	CommentPanel.AddCustomRow(FText::FromString("Commentary"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Commentary"))
		]
	.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(FText::FromString(GetDetailObject(&DetailBuilder)->Comment))
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FCustomNodeBaseDetail::CommentCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
}

UCustomNodeBase* FCustomNodeBaseDetail::GetDetailObject(IDetailLayoutBuilder* DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	DetailBuilder->GetObjectsBeingCustomized(OutObjects);
	UCustomNodeBase* Object = NULL;
	if (OutObjects.Num() > 0)
	{
		Object = Cast<UCustomNodeBase>(OutObjects[0].Get());
	}
	return Object;
}

void FCustomNodeBaseDetail::CommentCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UCustomNodeBase* Object = GetDetailObject(DetailBuilder);

	Object->Comment = NewText.ToString();
}


//FDialogNodeBaseDetail.............................................................................................

TSharedRef<IDetailCustomization> FDialogNodeBaseDetail::MakeInstance()
{
	return MakeShareable(new FDialogNodeBaseDetail);
}



void FDialogNodeBaseDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FCustomNodeBaseDetail::CustomizeDetails(DetailBuilder);

	FText TopDialog;
	if (((UDialogStartNode*)GetDetailObject(&DetailBuilder))->TopDialog)
	{
		TopDialog = ((UDialogStartNode*)GetDetailObject(&DetailBuilder))->TopDialog->Dialog;
	}
	TopDialog = UDialogNodeBase::GetDialogName(TopDialog);


	pMainPanel->AddCustomRow(FText::FromString("TopDialog"))
	
		.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("TopDialog"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(TopDialog)
			];
	pDialogOwnerProperty = &(pMainPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UDialogNodeBase, DialogOwner), UDialogNodeBase::StaticClass()));
	pDialogOwnerProperty->Visibility(EVisibility::Hidden);
}

//FDialogNodeDependetDetail.............................................................................................

TSharedRef<IDetailCustomization> FDialogDependetNodeDetail::MakeInstance()
{
	return MakeShareable(new FDialogDependetNodeDetail);
}

void FDialogDependetNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FDialogNodeBaseDetail::CustomizeDetails(DetailBuilder);

	
	if (GetDetailObject(&DetailBuilder)->pGraphObject)
	{
		pMainPanel->AddCustomRow(FText::FromString("Owner"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Owner"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(GetDetailObject(&DetailBuilder)->pGraphObject->ObjName)
			];
	}
	else
	{
		pMainPanel->AddCustomRow(FText::FromString("Owner"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Owner"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Not owner"))
			];
	}
}

//FStoryGraphDependetNodeDetail.............................................................................................

TSharedRef<IDetailCustomization> FStoryGraphDependetNodeDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphDependetNodeDetail);
}

void FStoryGraphDependetNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FCustomNodeBaseDetail::CustomizeDetails(DetailBuilder);

	if (GetDetailObject(&DetailBuilder)->pGraphObject)
	{
		pMainPanel->AddCustomRow(FText::FromString("Owner"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Owner"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(GetDetailObject(&DetailBuilder)->pGraphObject->ObjName)
			];
	}
	else
	{
		pMainPanel->AddCustomRow(FText::FromString("Owner"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Owner"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Not owner"))
			];
	}
}


//FDialogStartNodeDetail...........................................................


TSharedRef<IDetailCustomization> FDialogStartNodeDetail::MakeInstance()
{
	return MakeShareable(new FDialogStartNodeDetail);
}



void FDialogStartNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FDialogNodeBaseDetail::CustomizeDetails(DetailBuilder);

	pDialogOwnerProperty->Visibility(EVisibility::Visible);

	UDialogStartNode* DialogNode = (UDialogStartNode*)GetDetailObject(&DetailBuilder);
	
	
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory(FName(*FString("Dialog")), FText::GetEmpty());

	
	MyCategory.AddCustomRow(FText::FromString("Dialog "))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Full dialog"))

		]
		.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(DialogNode->Dialogs[0]->Dialog)
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FDialogStartNodeDetail::DialogCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];

		
	
}



void FDialogStartNodeDetail::DialogCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UDialogStartNode* Object = (UDialogStartNode*)GetDetailObject(DetailBuilder);

	Object->SetNewDialog(NewText);
	
}



//FDialogNodeDetail...........................................................


TSharedRef<IDetailCustomization> FDialogNodeDetail::MakeInstance()
{
	return MakeShareable(new FDialogNodeDetail);
}



void FDialogNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FDialogNodeBaseDetail::CustomizeDetails(DetailBuilder);

	pDialogOwnerProperty->Visibility(EVisibility::Visible);

	UDialogNode* DialogNode = (UDialogNode*)GetDetailObject(&DetailBuilder);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory(FName("Dialogs"), FText::GetEmpty());

	MyCategory.AddCustomRow(FText::FromString("Dialog 1"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Dialog 1"))

		]
		.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(DialogNode->Dialogs[0]->Dialog)
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FDialogNodeDetail::DialogCommitted, &DetailBuilder, 0))
			.AutoWrapText(true)
		];
	
		if (DialogNode->DialogOwner != ECharDialogOwner::NPC && DialogNode->NodeType != ENodeType::Message)
		{
			for (int i = 1; i < DialogNode->Dialogs.Num(); i++)
			{

				MyCategory.AddCustomRow(FText::FromString("Dialog " + FString::FromInt(i + 1)))
					.NameContent()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Dialog " + FString::FromInt(i + 1)))

					]
				.ValueContent()
					[
						SNew(SMultiLineEditableTextBox)
						.Text(DialogNode->Dialogs[i]->Dialog)
						.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FDialogNodeDetail::DialogCommitted, &DetailBuilder, i))
						.AutoWrapText(true)
					];

				
				MyCategory.AddCustomRow(FText::FromString("Remove dialog"))
					.ValueContent()
					[
						SNew(SButton)
						.Text(FText::FromString("Remove dialog"))
						.OnClicked(FOnClicked::CreateStatic(&FDialogNodeDetail::RemoveDialogButtonClick, &DetailBuilder, i))
					];
				
				
			}


			IDetailCategoryBuilder& AddButtonCategory = DetailBuilder.EditCategory("Add Dialog", FText::GetEmpty(), ECategoryPriority::Default);
			AddButtonCategory.AddCustomRow(FText::FromString("Add Dialog"))
				.ValueContent()
				[
					SNew(SButton)
					.Text(FText::FromString("Add Dialog"))
					.OnClicked(FOnClicked::CreateStatic(&FDialogNodeDetail::AddDialogButtonClick, &DetailBuilder))
				];
		}
}

FReply FDialogNodeDetail::AddDialogButtonClick(IDetailLayoutBuilder* DetailBuilder)
{
	UDialogNode* Object = (UDialogNode*)GetDetailObject(DetailBuilder);

	Object->AddDialog();
	
	DetailBuilder->ForceRefreshDetails(); //Update detail panel
	return FReply::Handled();
}

FReply FDialogNodeDetail::RemoveDialogButtonClick(IDetailLayoutBuilder* DetailBuilder, int Num)
{
	UDialogNode* Object = (UDialogNode*)GetDetailObject(DetailBuilder);

	Object->Dialogs.RemoveAt(Num);
	Object->RemovePin(Num + 1);

	DetailBuilder->ForceRefreshDetails(); //Update detail panel
	Object->UpdateGraphNode(); //Update graph node
	return FReply::Handled();
}


void FDialogNodeDetail::DialogCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder, int Num)
{
	UDialogNode* Object = (UDialogNode*)GetDetailObject(DetailBuilder);

	Object->Dialogs[Num]->Dialog = NewText;
	Object->UpdateGraphNode(); //Update graph node
}


//FGetStoryGraphObjectStateDetail.............................................................................................

TSharedRef<IDetailCustomization> FGetStoryGraphObjectStateNodeDetail::MakeInstance()
{
	return MakeShareable(new FGetStoryGraphObjectStateNodeDetail);
}

void FGetStoryGraphObjectStateNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphDependetNodeDetail::CustomizeDetails(DetailBuilder);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("Graph Object States", FText::GetEmpty(), ECategoryPriority::Default);

	
	MyCategory.AddCustomRow(FText::FromString("Quest phase"))
	.ValueContent() //NameContent()
	[
		SAssignNew(ObjectStatesListBox, SMenuAnchor)
			.OnGetMenuContent(this, &FGetStoryGraphObjectStateNodeDetail::GetGraphObjectStateMenuOptions)
			.Placement(MenuPlacement_ComboBox)
			[
				SNew(SButton)
					.ClickMethod(EButtonClickMethod::MouseDown)
					.ContentPadding(FMargin(5.0f, 2.0f))
					.VAlign(VAlign_Center)
					.ButtonStyle(FEditorStyle::Get(), "ViewPortMenu.Button")
					.OnClicked(this, &FGetStoryGraphObjectStateNodeDetail::HandleGraphObjectStateListButtonClicked)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(WantedObjectStateTextBox, STextBlock)
								.Text(GetDetailObject(&DetailBuilder)->GetNodeTitle())
								.TextStyle(FEditorStyle::Get(), "ViewportMenu.Label")
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Padding(2.0f)
							[
								SNew(SImage)
								.Image(FEditorStyle::GetBrush("ComboButton.Arrow"))
								.ColorAndOpacity(FLinearColor::Black)
							]
					]
			]
	];

}


FReply FGetStoryGraphObjectStateNodeDetail::HandleGraphObjectStateListButtonClicked()
{
	if (ObjectStatesListBox->ShouldOpenDueToClick())
	{
		ObjectStatesListBox->SetIsOpen(true);
	}
	else
	{
		ObjectStatesListBox->SetIsOpen(false);
	}

	return FReply::Handled();
}

TSharedRef<SWidget> FGetStoryGraphObjectStateNodeDetail::GetGraphObjectStateMenuOptions()
{
	UCustomNodeBase* ObjectOwner = GetDetailObject(pDetailBuilder);
	FMenuBuilder LayerMenuBuilder(true, NULL);
	TArray<FString> ObjectStates;
	if (ObjectOwner->pGraphObject)
	{
		UStoryGraphObject* pStoryGraphObject = (UStoryGraphObject*)ObjectOwner->pGraphObject;
		pStoryGraphObject->GetObjectStateAsString(ObjectStates);

		for (int i = 0; i < ObjectStates.Num(); i++)
		{
			FUIAction LayerActive(FExecuteAction::CreateSP(this, &FGetStoryGraphObjectStateNodeDetail::HandleSetCurrentGraphObjectState, i));
			LayerMenuBuilder.AddMenuEntry(FText::FromString(ObjectStates[i]), FText::FromString("Choose state."), FSlateIcon(), LayerActive);
		}
	}
	return LayerMenuBuilder.MakeWidget();
}

void FGetStoryGraphObjectStateNodeDetail::HandleSetCurrentGraphObjectState(int WantedState)
{
	TArray<FString> ObjectStates;
	UStoryGraphObject* pStoryGraphObject = GetDetailObject(pDetailBuilder)->pGraphObject;
	pStoryGraphObject->GetObjectStateAsString(ObjectStates);

	if (UGetStoryGraphObjectStateNode* Node = Cast<UGetStoryGraphObjectStateNode>(GetDetailObject(pDetailBuilder)))
	{
		Node->SetWantedObjectState(WantedState);
	}
	
	WantedObjectStateTextBox->SetText(ObjectStates[WantedState]);
}

//FAddDialogNodeDetail..............................................................................................
TSharedRef<IDetailCustomization> FAddDialogNodeDetail::MakeInstance()
{
	return MakeShareable(new FAddDialogNodeDetail);
}

void FAddDialogNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphDependetNodeDetail::CustomizeDetails(DetailBuilder);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("Add Dialog", FText::GetEmpty(), ECategoryPriority::Default);

	
	MyCategory.AddCustomRow(FText::FromString("Add Dialog"))
	.ValueContent()
	[
		SAssignNew(ObjectStatesListBox, SMenuAnchor)
			.OnGetMenuContent(this, &FAddDialogNodeDetail::GetAvailableDialogs)
			.Placement(MenuPlacement_ComboBox)
			[
				SNew(SButton)
					.ClickMethod(EButtonClickMethod::MouseDown)
					.ContentPadding(FMargin(5.0f, 2.0f))
					.VAlign(VAlign_Center)
					.ButtonStyle(FEditorStyle::Get(), "ViewPortMenu.Button")
					.OnClicked(this, &FAddDialogNodeDetail::HandleAvailableDialogsButtonClicked)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(SelectedDialogTextBox, STextBlock)
								.Text(GetDetailObject(&DetailBuilder)->GetNodeTitle())
								.TextStyle(FEditorStyle::Get(), "ViewportMenu.Label")
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Padding(2.0f)
							[
								SNew(SImage)
								.Image(FEditorStyle::GetBrush("ComboButton.Arrow"))
								.ColorAndOpacity(FLinearColor::Black)
							]
					]
			]
	];

}

FReply FAddDialogNodeDetail::HandleAvailableDialogsButtonClicked()
{
	if (ObjectStatesListBox->ShouldOpenDueToClick())
	{
		ObjectStatesListBox->SetIsOpen(true);
	}
	else
	{
		ObjectStatesListBox->SetIsOpen(false);
	}

	return FReply::Handled();
}

TSharedRef<SWidget> FAddDialogNodeDetail::GetAvailableDialogs()
{
	UAddDialogNode* AddDialogNode = Cast<UAddDialogNode>(GetDetailObject(pDetailBuilder));

	FMenuBuilder LayerMenuBuilder(true, NULL);
	TArray<UDialogStartNode*> DialogRootNode;

	if (AddDialogNode && AddDialogNode->pGraphObject)
	{
		UStoryGraphCharecter* pGraphCharecter = Cast<UStoryGraphCharecter>(AddDialogNode->pGraphObject);
		UStoryGraphPlaceTrigger* pPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(AddDialogNode->pGraphObject);
		if (pGraphCharecter)
		{
			for (int i = 0; i < pGraphCharecter->GarphNods.Num(); i++)
			{
				if (UDialogStartNode* DialogStartNode = Cast<UDialogStartNode>(pGraphCharecter->GarphNods[i]))
				{
					DialogRootNode.Add(DialogStartNode);
				}
			}

		}
		else if (pPlaceTrigger)
		{
			for (int i = 0; i < pPlaceTrigger->GarphNods.Num(); i++)
			{
				if (UDialogStartNode* DialogStartNode = Cast<UDialogStartNode>(pPlaceTrigger->GarphNods[i]))
				{
					DialogRootNode.Add(DialogStartNode);
				}
			}
		}

	}
	

	for (int i = 0; i < DialogRootNode.Num(); i++)
	{
		FUIAction LayerActive(FExecuteAction::CreateSP(this, &FAddDialogNodeDetail::HandleSetCurrentDialog, DialogRootNode[i]));
		LayerMenuBuilder.AddMenuEntry(UDialogNodeBase::GetDialogName(DialogRootNode[i]->Dialogs[0]->Dialog), FText::FromString("Choose dialog."), FSlateIcon(), LayerActive);
	}
	return LayerMenuBuilder.MakeWidget();
}

void FAddDialogNodeDetail::HandleSetCurrentDialog(UDialogStartNode* SelectedDialog)
{
	UAddDialogNode* ObjectOwner = (UAddDialogNode*)GetDetailObject(pDetailBuilder);
	
	ObjectOwner->SetCurentDialog(SelectedDialog);
	SelectedDialogTextBox->SetText(UDialogNodeBase::GetDialogName(SelectedDialog->Dialogs[0]->Dialog));
}

//FAddDialogFromDialogDetail..............................................................................................
TSharedRef<IDetailCustomization> FAddDialogFromDialogDetail::MakeInstance()
{
	return MakeShareable(new FAddDialogFromDialogDetail);
}

void FAddDialogFromDialogDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FDialogNodeBaseDetail::CustomizeDetails(DetailBuilder);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("Add Dialog", FText::GetEmpty(), ECategoryPriority::Default);

	
	MyCategory.AddCustomRow(FText::FromString("Add Dialog"))
	.ValueContent()
	[
		SAssignNew(ObjectStatesListBox, SMenuAnchor)
			.OnGetMenuContent(this, &FAddDialogFromDialogDetail::GetAvailableDialogs)
			.Placement(MenuPlacement_ComboBox)
			[
				SNew(SButton)
					.ClickMethod(EButtonClickMethod::MouseDown)
					.ContentPadding(FMargin(5.0f, 2.0f))
					.VAlign(VAlign_Center)
					.ButtonStyle(FEditorStyle::Get(), "ViewPortMenu.Button")
					.OnClicked(this, &FAddDialogFromDialogDetail::HandleAvailableDialogsButtonClicked)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(SelectedDialogTextBox, STextBlock)
								.Text(GetDetailObject(&DetailBuilder)->GetNodeTitle())
								.TextStyle(FEditorStyle::Get(), "ViewportMenu.Label")
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Padding(2.0f)
							[
								SNew(SImage)
								.Image(FEditorStyle::GetBrush("ComboButton.Arrow"))
								.ColorAndOpacity(FLinearColor::Black)
							]
					]
			]
	];

}

FReply FAddDialogFromDialogDetail::HandleAvailableDialogsButtonClicked()
{
	if (ObjectStatesListBox->ShouldOpenDueToClick())
	{
		ObjectStatesListBox->SetIsOpen(true);
	}
	else
	{
		ObjectStatesListBox->SetIsOpen(false);
	}

	return FReply::Handled();
}

TSharedRef<SWidget> FAddDialogFromDialogDetail::GetAvailableDialogs()
{
	
	UAddDialogFromDialogNode* AddDialogFromDialogNode = Cast<UAddDialogFromDialogNode>(GetDetailObject(pDetailBuilder));

	FMenuBuilder LayerMenuBuilder(true, NULL);
	TArray<UDialogStartNode*> DialogRootNode;
	

	if (AddDialogFromDialogNode)
	{
		for (int i = 0; i < ((UStoryGraphCharecter*)AddDialogFromDialogNode->GetOuter())->GarphNods.Num(); i++)
		{
			if (UDialogStartNode* DialogStart = Cast<UDialogStartNode>(((UStoryGraphCharecter*)AddDialogFromDialogNode->GetOuter())->GarphNods[i]))
			{
				DialogRootNode.Add(DialogStart);
			}
		}
	}

	for (int i = 0; i < DialogRootNode.Num(); i++)
	{
		FUIAction LayerActive(FExecuteAction::CreateSP(this, &FAddDialogFromDialogDetail::HandleSetCurrentDialog, DialogRootNode[i]));
		LayerMenuBuilder.AddMenuEntry(UDialogNodeBase::GetDialogName(DialogRootNode[i]->Dialogs[0]->Dialog), FText::FromString("Choose dialog."), FSlateIcon(), LayerActive);
	}
	return LayerMenuBuilder.MakeWidget();
}

void FAddDialogFromDialogDetail::HandleSetCurrentDialog(UDialogStartNode* SelectedDialog)
{
	UAddDialogFromDialogNode* AddDialogFromDialogNode = Cast<UAddDialogFromDialogNode>(GetDetailObject(pDetailBuilder));
	
	AddDialogFromDialogNode->SetCurentDialog(SelectedDialog);
	SelectedDialogTextBox->SetText(UDialogNodeBase::GetDialogName(SelectedDialog->Dialogs[0]->Dialog));
}


//FAddQuestPhaseNodeDetail...........................................................


TSharedRef<IDetailCustomization> FAddQuestPhaseNodeDetail::MakeInstance()
{
	return MakeShareable(new FAddQuestPhaseNodeDetail);
}



void FAddQuestPhaseNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphDependetNodeDetail::CustomizeDetails(DetailBuilder);


	UAddQuestPhaseNode* QuestPhaseNode = (UAddQuestPhaseNode*)GetDetailObject(&DetailBuilder);
	
	
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory(FName(*FString("Quest Phase")), FText::GetEmpty());

	MyCategory.AddCustomRow(FText::FromString("Owned Quest"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Owned Quest"))

		]
		.ValueContent()
		[
			SNew(STextBlock)
			.Text(QuestPhaseNode->QuestPhaseToAdd->pOwnedQuest ? QuestPhaseNode->QuestPhaseToAdd->pOwnedQuest->ObjName : FText::FromString("Non"))
		];
	
	MyCategory.AddCustomRow(FText::FromString("Quest Phase"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Quest Phase"))

		]
		.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(QuestPhaseNode->QuestPhaseToAdd->Decription)
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FAddQuestPhaseNodeDetail::QuestPhaseCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
	
		
		
}



void FAddQuestPhaseNodeDetail::QuestPhaseCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UAddQuestPhaseNode* Object = (UAddQuestPhaseNode*)GetDetailObject(DetailBuilder);

	Object->SetQuestPhase(NewText);
	
}

//FQuestStarNodeDetail...........................................................


TSharedRef<IDetailCustomization> FQuestStarNodeDetail::MakeInstance()
{
	return MakeShareable(new FQuestStarNodeDetail);
}



void FQuestStarNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphDependetNodeDetail::CustomizeDetails(DetailBuilder);


	UQuestStartNode* QuestPhaseNode = (UQuestStartNode*)GetDetailObject(&DetailBuilder);


	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory(FName(*FString("Quest property")), FText::GetEmpty());

	TArray<UObject*> Objects;
	Objects.Add(QuestPhaseNode->pGraphObject);
	MyCategory.AddExternalObjectProperty(Objects,GET_MEMBER_NAME_CHECKED(UStoryGraphQuest, MainQuest));

}


//FSetInventoryItemStateNodeDetail..............................................................................................
TSharedRef<IDetailCustomization> FSetInventoryItemStateNodeDetail::MakeInstance()
{
	return MakeShareable(new FSetInventoryItemStateNodeDetail);
}

void FSetInventoryItemStateNodeDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphDependetNodeDetail::CustomizeDetails(DetailBuilder);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("Set phase", FText::GetEmpty(), ECategoryPriority::Default);

	
	MyCategory.AddCustomRow(FText::FromString("Set phase"))
	.ValueContent()
	[
		SAssignNew(InventoryItemPhaseListBox, SMenuAnchor)
			.OnGetMenuContent(this, &FSetInventoryItemStateNodeDetail::GetAvailableInventoryItemPhase)
			.Placement(MenuPlacement_ComboBox)
			[
				SNew(SButton)
					.ClickMethod(EButtonClickMethod::MouseDown)
					.ContentPadding(FMargin(5.0f, 2.0f))
					.VAlign(VAlign_Center)
					.ButtonStyle(FEditorStyle::Get(), "ViewPortMenu.Button")
					.OnClicked(this, &FSetInventoryItemStateNodeDetail::HandleAvailableDialogsButtonClicked)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(SelectedInventoryItemPhaseTextBox, STextBlock)
								.Text(GetDetailObject(&DetailBuilder)->GetNodeTitle())
								.TextStyle(FEditorStyle::Get(), "ViewportMenu.Label")
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Padding(2.0f)
							[
								SNew(SImage)
								.Image(FEditorStyle::GetBrush("ComboButton.Arrow"))
								.ColorAndOpacity(FLinearColor::Black)
							]
					]
			]
	];

}

FReply FSetInventoryItemStateNodeDetail::HandleAvailableDialogsButtonClicked()
{
	if (InventoryItemPhaseListBox->ShouldOpenDueToClick())
	{
		InventoryItemPhaseListBox->SetIsOpen(true);
	}
	else
	{
		InventoryItemPhaseListBox->SetIsOpen(false);
	}

	return FReply::Handled();
}

TSharedRef<SWidget> FSetInventoryItemStateNodeDetail::GetAvailableInventoryItemPhase()
{
	UCustomNodeBase* ObjectOwner = (UCustomNodeBase*)GetDetailObject(pDetailBuilder);

	FMenuBuilder LayerMenuBuilder(true, NULL);
	TArray<UDialogStartNode*> DialogRootNode;
	

	TArray<FString> ObjectStates;
	if (ObjectOwner->pGraphObject)
	{
		UStoryGraphObject* pStoryGraphObject = (UStoryGraphObject*)ObjectOwner->pGraphObject;
		pStoryGraphObject->GetObjectStateAsString(ObjectStates);

		for (int i = 0; i < ObjectStates.Num(); i++)
		{
			FUIAction LayerActive(FExecuteAction::CreateSP(this, &FSetInventoryItemStateNodeDetail::HandleSetCurrentInventoryItemState, i));
			LayerMenuBuilder.AddMenuEntry(FText::FromString(ObjectStates[i]), FText::FromString("Choose state."), FSlateIcon(), LayerActive);
		}
	}
	return LayerMenuBuilder.MakeWidget();
}

void FSetInventoryItemStateNodeDetail::HandleSetCurrentInventoryItemState(int StateNumber)
{
	TArray<FString> ObjectStates;
	USetInventoryItemStateNode* ObjectOwner = Cast<USetInventoryItemStateNode>(GetDetailObject(pDetailBuilder));
	if (ObjectOwner)
	{
		ObjectOwner->SetCurrentState(StateNumber);

		ObjectOwner->pGraphObject->GetObjectStateAsString(ObjectStates);
		SelectedInventoryItemPhaseTextBox->SetText(ObjectStates[StateNumber]);
	}
}
