// Copyright 2016 Dmitriy Pavlov

#include "DetailPanelCustomizationObject.h"
#include "PropertyEditorModule.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "StoryGraphObject.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "GraphSchema_StoryGraph.h"
#include "StoryGraph.h"
#include "CustomNods.h"
#include "DetailLayoutBuilder.h"
#include "AssetEditor_StoryGraph.h"

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
	UStoryGraphObject* Object = NULL;
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


//FStoryGraphObjectWithScenObjectDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphObjectWithScenObjectDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphObjectWithScenObjectDetail);
}



void FStoryGraphObjectWithScenObjectDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel = &DetailBuilder.EditCategory("Scen objects links", FText::GetEmpty(), ECategoryPriority::Important);
	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObjectWithScenObject, IsScenObjectActive), UStoryGraphObjectWithScenObject::StaticClass());
	
}



//FStoryGraphCharecterDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphCharecterDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphCharecterDetail);
}



void FStoryGraphCharecterDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectWithScenObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphCharecter, ScenCharecters), UStoryGraphCharecter::StaticClass());

	UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(GetDetailObject(&DetailBuilder));

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
			.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FStoryGraphCharecterDetail::DefaultAnswerCommitted, &DetailBuilder))
			.AutoWrapText(true)
		];
	CharecterPanel.AddCustomRow(FText::FromString("Edit Dialog"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Edit Dialog"))
			.OnClicked(FOnClicked::CreateStatic(&FStoryGraphCharecterDetail::EditDialogButtonClick, &DetailBuilder))
		];

}



FReply FStoryGraphCharecterDetail::EditDialogButtonClick(IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphCharecter* Object = (UStoryGraphCharecter*)GetDetailObject(DetailBuilder);
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Edit Dialog"));
	UStoryGraph* StoryGraph = Cast<UStoryGraph>(Object->GetOuter());
	StoryGraph->pAssetEditor->OpenDialogEditorTab(Object);

	return FReply::Handled();
}

void FStoryGraphCharecterDetail::DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder)
{
	UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(GetDetailObject(DetailBuilder));

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
	FStoryGraphObjectWithScenObjectDetail::CustomizeDetails(DetailBuilder);

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

	IDetailCategoryBuilder* ScenObjectsPanel = &DetailBuilder.EditCategory("Scen objects links", FText::GetEmpty(), ECategoryPriority::Important);

	ScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphInventoryItem, InventoryItemWithoutScenObject), UStoryGraphInventoryItem::StaticClass());
	
	IDetailPropertyRow* IsScenObjectActiveProperty = &ScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphObjectWithScenObject, IsScenObjectActive), UStoryGraphObjectWithScenObject::StaticClass());
	IDetailPropertyRow* ScenInventoryItemsProperty = &ScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphInventoryItem, ScenInventoryItems), UStoryGraphInventoryItem::StaticClass());
	if (!InventoryItem->InventoryItemWithoutScenObject)
	{
		IsScenObjectActiveProperty->IsEnabled(true);
		ScenInventoryItemsProperty->IsEnabled(true);
	}
	else
	{
		IsScenObjectActiveProperty->IsEnabled(false);
		ScenInventoryItemsProperty->IsEnabled(false);
	}
	

}

//FStoryGraphOthersDetail...........................................................

TSharedRef<IDetailCustomization> FStoryGraphOthersDetail::MakeInstance()
{
	return MakeShareable(new FStoryGraphOthersDetail);
}



void FStoryGraphOthersDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FStoryGraphObjectWithScenObjectDetail::CustomizeDetails(DetailBuilder);

	pScenObjectsPanel->AddProperty(GET_MEMBER_NAME_CHECKED(UStoryGraphOthers, ScenOtherObjects), UStoryGraphOthers::StaticClass());

}