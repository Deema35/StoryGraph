// Copyright 2016 Dmitriy Pavlov

#include "DetailPanelCustomizationObject.h"
#include "AssetEditor_StoryGraph.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "GraphSchema_StoryGraph.h"
#include "PropertyEditorModule.h"
#include "StoryGraph.h"
#include "StoryGraphObject.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

//FStoryGraphObjectDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphObjectDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphObjectDetail);
}



void FStoryGraphObjectDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	pMainPanel = &(DetailBuilder.EditCategory("Main",FText::GetEmpty(),ECategoryPriority::Important));
	IDetailCategoryBuilder& CommentPanel = DetailBuilder.EditCategory("Commentary", FText::GetEmpty(), ECategoryPriority::Uncommon);
	
	pMainPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObject, ObjName), UStoryGraphObject::StaticClass());
	pMainPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObject, Category), UStoryGraphObject::StaticClass());

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
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FStoryGraphObjectDetail::CommentCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
}

UStoryGraphObject* FStoryGraphObjectDetail::GetDetailObject(IDetailLayoutBuilder* DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	DetailBuilder->GetObjectsBeingCustomized(OutObjects);
	UStoryGraphObject* Object = nullptr;
	if (OutObjects.Num() > 0)
	{
		Object = Cast<UStoryGraphObject>(OutObjects[0].Get());
	}
	return Object;
}

void FStoryGraphObjectDetail::CommentCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphObject* Object = GetDetailObject(DetailBuilder);

	Object->Comment = NewText.ToString();
}


//FStoryGraphObjectWithSceneObjectDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphObjectWithSceneObjectDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphObjectWithSceneObjectDetail);
}



void FStoryGraphObjectWithSceneObjectDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel = &DetailBuilder.EditCategory("Scen objects links", FText::GetEmpty(), ECategoryPriority::Important);
	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObjectWithSceneObject, IsSceneObjectActive), UStoryGraphObjectWithSceneObject::StaticClass());
	
}



//FStoryGraphCharacterDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphCharacterDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphCharacterDetail);
}



void FStoryGraphCharacterDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectWithSceneObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphCharacter, SceneCharacters), UStoryGraphCharacter::StaticClass());

	UStoryGraphCharacter* Charecter = Cast<UStoryGraphCharacter>(GetDetailObject(&DetailBuilder));

	IDetailCategoryBuilder& CharecterPanel = DetailBuilder.EditCategory("Charecter property", FText::GetEmpty(), ECategoryPriority::Important);

	CharecterPanel.AddCustomRow(FText::FromString("Default Answer"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Default answer"))

		]
	.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(Charecter->DefaultAnswer)
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FStoryGraphCharacterDetail::DefaultAnswerCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
	CharecterPanel.AddCustomRow(FText::FromString("Edit Dialog"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Edit Dialog"))
			.OnClicked(FOnClicked::CreateStatic(&FStoryGraphCharacterDetail::EditDialogButtonClick, &DetailBuilder))
		];

}



FReply FStoryGraphCharacterDetail::EditDialogButtonClick(IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphCharacter* Object = (UStoryGraphCharacter*)GetDetailObject(DetailBuilder);
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Edit Dialog"));
	UStoryGraph* StoryGraph = Cast<UStoryGraph>(Object->GetOuter());
	StoryGraph->pAssetEditor->OpenDialogEditorTab(Object);

	return FReply::Handled();
}

void FStoryGraphCharacterDetail::DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphCharacter* Charecter = Cast<UStoryGraphCharacter>(GetDetailObject(DetailBuilder));

	Charecter->DefaultAnswer = NewText;

	Charecter->Modify();
}

//FStoryGraphPlaceTriggerDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphPlaceTriggerDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphPlaceTriggerDetail);
}



void FStoryGraphPlaceTriggerDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectWithSceneObjectDetail::CustomizeDetails(DetailBuilder);

	UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(GetDetailObject(&DetailBuilder));

	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphPlaceTrigger, ScenTriggers), UStoryGraphPlaceTrigger::StaticClass());

	IDetailCategoryBuilder& PlaceTriggerPanel = DetailBuilder.EditCategory("PlaceTrigger property", FText::GetEmpty(), ECategoryPriority::Important);

	PlaceTriggerPanel.AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphPlaceTrigger, PlaceTriggerType));

	PlaceTriggerPanel.AddCustomRow(FText::FromString("Default Answer"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Default answer"))

		]
		.ValueContent()
		[
			SNew(SMultiLineEditableTextBox)
			.Text(PlaceTrigger->DefaultAnswer)
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FStoryGraphPlaceTriggerDetail::DefaultAnswerCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
	PlaceTriggerPanel.AddCustomRow(FText::FromString("Edit Message graph"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Edit Message"))
			.OnClicked(FOnClicked::CreateStatic(&FStoryGraphPlaceTriggerDetail::EditMessageButtonClick, &DetailBuilder))
		];

}



FReply FStoryGraphPlaceTriggerDetail::EditMessageButtonClick(IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphPlaceTrigger* Object = (UStoryGraphPlaceTrigger*)GetDetailObject(DetailBuilder);
	
	UStoryGraph* StoryGraph = Cast<UStoryGraph>(Object->GetOuter());
	StoryGraph->pAssetEditor->OpenDialogEditorTab(Object);

	return FReply::Handled();
}

void FStoryGraphPlaceTriggerDetail::DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(GetDetailObject(DetailBuilder));

	PlaceTrigger->DefaultAnswer = NewText;

	PlaceTrigger->Modify();
}

//FStoryGraphInventoryItemDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphInventoryItemDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphInventoryItemDetail);
}



void FStoryGraphInventoryItemDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectDetail::CustomizeDetails(DetailBuilder);

	UStoryGraphInventoryItem* InventoryItem = Cast<UStoryGraphInventoryItem>(GetDetailObject(&DetailBuilder));

	IDetailCategoryBuilder* SceneObjectsPanel = &DetailBuilder.EditCategory("Scene objects links", FText::GetEmpty(), ECategoryPriority::Important);

	SceneObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphInventoryItem, InventoryItemWithoutSceneObject), UStoryGraphInventoryItem::StaticClass());
	
	IDetailPropertyRow* IsSceneObjectActiveProperty = &SceneObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObjectWithSceneObject, IsSceneObjectActive), UStoryGraphObjectWithSceneObject::StaticClass());
	IDetailPropertyRow* SceneInventoryItemsProperty = &SceneObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphInventoryItem, SceneInventoryItems), UStoryGraphInventoryItem::StaticClass());
	if (!InventoryItem->InventoryItemWithoutSceneObject)
	{
		IsSceneObjectActiveProperty->IsEnabled(true);
		SceneInventoryItemsProperty->IsEnabled(true);
	}
	else
	{
		IsSceneObjectActiveProperty->IsEnabled(false);
		SceneInventoryItemsProperty->IsEnabled(false);
	}
	

}

//FStoryGraphOthersDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphOthersDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphOthersDetail);
}



void FStoryGraphOthersDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectWithSceneObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphOthers, SceneOtherObjects), UStoryGraphOthers::StaticClass());

}