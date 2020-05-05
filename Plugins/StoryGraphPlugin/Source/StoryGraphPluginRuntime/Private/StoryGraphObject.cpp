// Copyright 2016 Dmitriy Pavlov

#include "StoryGraphObject.h"

#include "Character_StoryGraph.h"
#include "CustomNodes.h"
#include "HUD_StoryGraph.h"
#include "StoryGraph.h"
#include "StoryGraphWidget.h"
#include "InventoryItem_StoryGraph.h"
#include "PlaceTrigger_StoryGraph.h"
#include "OtherActor_StoryGraph.h"
#if WITH_EDITORONLY_DATA
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#endif //WITH_EDITORONLY_DATA

int UStoryGraphObject::CharNum;

int UStoryGraphObject::QuestNum;

int UStoryGraphObject::PlaceTriggerNum;

int UStoryGraphObject::DialogTriggerNum;

int UStoryGraphObject::OthersNum;

int UStoryGraphObject::InventoryItemNum;


UStoryGraphObject::UStoryGraphObject()
{
	Category = "Default";
	ObjName = FText::FromString("New Object" + FString::FromInt(OthersNum++));
	ObjectType = EStoryObjectType::Non;
}

TSubclassOf<UStoryGraphObject> UStoryGraphObject::GetClassFromStoryObjectType(EStoryObjectType EnumValue)
{
	switch (EnumValue)
	{
	case EStoryObjectType::Character:

		return UStoryGraphCharacter::StaticClass();

	case EStoryObjectType::Quest:

		return UStoryGraphQuest::StaticClass();

	case EStoryObjectType::DialogTrigger:

		return UStoryGraphDialogTrigger::StaticClass();

	case EStoryObjectType::PlaceTrigger:

		return UStoryGraphPlaceTrigger::StaticClass();

	case EStoryObjectType::InventoryItem:

		return UStoryGraphInventoryItem::StaticClass();

	case EStoryObjectType::Others:

		return UStoryGraphOthers::StaticClass();

	default:

		return UStoryGraphObject::StaticClass();
	}
}

FString UStoryGraphObject::GetObjectTypeEnumAsString(EStoryObjectType EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStoryObjectType"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetNameStringByIndex((int)EnumValue);
}

FString UStoryGraphObject::GetObjectToolTip(EStoryObjectType EnumValue)
{
	switch (EnumValue)
	{
	case EStoryObjectType::Character:

		return "Character has dialogs.";

	case EStoryObjectType::Quest:

		return "List of all available quests. After you added quest, will available start quest node.";

	case EStoryObjectType::DialogTrigger:

		return "Special facility for interaction dialogue(meesage) graph and main graph.";

	case EStoryObjectType::PlaceTrigger:

		return "PlaceTrigger - interactive object on map.";

	case EStoryObjectType::InventoryItem:

		return "Story object which can be added to inventory.";

	case EStoryObjectType::Others:

		return "Objects that do not have states. But can get messages.";

	default:

		return "Non.";
	}
}

void UStoryGraphObject::SetCurrentState(int NewState)
{
	ObjectState = NewState;
	((UStoryGraph*)GetOuter())->RefreshExecutionTrees();
}

void UStoryGraphObject::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Properties.clear();

	Properties.insert(std::pair<FString, XMLProperty>("ObjName", XMLProperty(ObjName.ToString())));
	Properties.insert(std::pair<FString, XMLProperty>("ObjectType",
	                                                  XMLProperty(
		                                                  GetEnumValueAsString<EStoryObjectType>(
			                                                  "EStoryObjectType", ObjectType))));
	Properties.insert(std::pair<FString, XMLProperty>("Category", XMLProperty(Category)));
	Properties.insert(std::pair<FString, XMLProperty>("Comment", XMLProperty(Comment)));
}

void UStoryGraphObject::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	ObjName = FText::FromString(Properties["ObjName"].Val);
	Category = Properties["Category"].Val;
	Comment = Properties["Comment"].Val;
}

//UStoryGraphObjectWithSceneObject.........................................................

UStoryGraphObjectWithSceneObject::UStoryGraphObjectWithSceneObject()
{
	IsSceneObjectActive = true;
	DependedNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependedNodes.Add(ENodeType::SetSceneObjectActive);
	DependedNodes.Add(ENodeType::SendMessageScene);
	DependedNodes.Add(ENodeType::AddTargetObjectToPhase);
}

void UStoryGraphObjectWithSceneObject::InitializeObject()
{
	SetActiveStateOfSceneObjects();
	TArray<IStorySceneObject*> ScenObjects;
	GetSceneObjects(ScenObjects);
	for (int i = 0; i < ObjectMessageStack.Num(); i++)
	{
		for (int j = 0; j < ScenObjects.Num(); j++)
		{
			ScenObjects[j]->SendMessageToSceneObject(ObjectMessageStack[i]);
		}
	}
}

void UStoryGraphObjectWithSceneObject::SetActiveStateOfSceneObjects()
{
	TArray<IStorySceneObject*> SceneObjects;

	GetSceneObjects(SceneObjects);
	for (int i = 0; i < SceneObjects.Num(); i++)
	{
		SceneObjects[i]->RefreshSceneObjectsActive();
	}
}

void UStoryGraphObjectWithSceneObject::SetSceneObjectActive(bool Active)
{
	IsSceneObjectActive = Active;
	SetActiveStateOfSceneObjects();
}

void UStoryGraphObjectWithSceneObject::SendMessageToSceneObject(FString Message)
{
	TArray<IStorySceneObject*> SceneObjects;
	GetSceneObjects(SceneObjects);
	for (int i = 0; i < SceneObjects.Num(); i++)
	{
		SceneObjects[i]->SendMessageToSceneObject(Message);
	}

	ObjectMessageStack.Add(Message);
}

void UStoryGraphObjectWithSceneObject::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);
	Properties.insert(
		std::pair<FString, XMLProperty>("IsSceneObjectActive", XMLProperty(IsSceneObjectActive ? "true" : "false")));
}

void UStoryGraphObjectWithSceneObject::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Super::LoadPropertyFromXML(Properties);
	IsSceneObjectActive = Properties["IsSceneObjectActive"].Val == "true" ? true : false;
}

//UStoryGraphCharacter.................................................................................

UStoryGraphCharacter::UStoryGraphCharacter()
{
	ObjName = FText::FromString("New Character" + FString::FromInt(CharNum++));
	ObjectType = EStoryObjectType::Character;
	DependedNodes.Add(ENodeType::AddDialog);
}

void UStoryGraphCharacter::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("ECharacterStates"))
	{
		States.Add(GetEnumValueAsString<ECharacterStates>("ECharacterStates", (ECharacterStates)i++));
	}
}

void UStoryGraphCharacter::GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects)
{
	TExtractSceneObjects<IStorySceneObject, ACharacter_StoryGraph>(SceneObjects, SceneCharacters,
	                                                               SceneCharacterPointers);
}

void UStoryGraphCharacter::GetSceneObjects(TArray<AActor*>& SceneObjects)
{
	TExtractSceneObjects<AActor, ACharacter_StoryGraph>(SceneObjects, SceneCharacters, SceneCharacterPointers);
}

void UStoryGraphCharacter::SetSceneObjectRealPointers()
{
	SceneCharacterPointers.Empty();
	for (int i = 0; i < SceneCharacters.Num(); i++)
	{
		SceneCharacterPointers.Add(SceneCharacters[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphCharacter::ClearSceneObjects()
{
	SceneCharacters.Empty();
}

void UStoryGraphCharacter::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	for (int i = 0; i < GraphNodes.Num(); i++)
	{
		if (Cast<UDialogStartNode>(GraphNodes[i]))
		{
			Objects.Add(GraphNodes[i]);
		}
	}
}

void UStoryGraphCharacter::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);
	Properties.insert(std::pair<FString, XMLProperty>("DefaultAnswer", XMLProperty(DefaultAnswer.ToString())));
}

void UStoryGraphCharacter::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Super::LoadPropertyFromXML(Properties);
	DefaultAnswer = FText::FromString(Properties["DefaultAnswer"].Val);
}

//UStoryGraphQuest.................................................................................

UStoryGraphQuest::UStoryGraphQuest()
{
	ObjName = FText::FromString("New Quest" + FString::FromInt(QuestNum++));
	ObjectType = EStoryObjectType::Quest;
	MainQuest = true;

	DependedNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependedNodes.Add(ENodeType::QuestStart);
	DependedNodes.Add(ENodeType::CancelQuest);
}

void UStoryGraphQuest::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("EQuestStates"))
	{
		States.Add(GetEnumValueAsString<EQuestStates>("EQuestStates", (EQuestStates)i++));
	}
}


void UStoryGraphQuest::SetCurrentState(int NewState)
{
	FText Message;

	ObjectState = NewState;

	switch ((EQuestStates)NewState)
	{
	case EQuestStates::Active:

		Message = FText::Format(NSLOCTEXT("StoryGraph", "Quest active", "New quest {0}"), ObjName);

		break;

	case EQuestStates::Canceled:

		Message = FText::Format(NSLOCTEXT("StoryGraph", "Quest cancel", "Quest {0} cancel"), ObjName);

		break;

	case EQuestStates::Complete:

		Message = FText::Format(NSLOCTEXT("StoryGraph", "Quest complite", "Quest {0} complite"), ObjName);

		break;
	}

	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
		((UStoryGraph*)GetOuter())->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		if (HUD->GameScreen)
		{
			HUD->GameScreen->AddMessageOnScreen(Message, 5);
		}
	}


	((UStoryGraph*)GetOuter())->QuestStateWasChange = true;
}

void UStoryGraphQuest::AddPhase(UQuestPhase* Phase)
{
	if (QuestPhase.Num() == 0)
	{
		SetCurrentState((int)EQuestStates::Active);
	}
	else
	{
		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
			(((UStoryGraph*)GetOuter())->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD())))
		{
			FText Mesg = FText::Format(NSLOCTEXT("StoryGraph", "AddQuestPhaseMessage2", "Quest {0} changed"), ObjName);

			if (HUD->GameScreen)
			{
				HUD->GameScreen->AddMessageOnScreen(Mesg, 5);
			}
		}
	}

	QuestPhase.Add(Phase);
}

void UStoryGraphQuest::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("MainQuest", XMLProperty(MainQuest ? "true" : "false")));
}

void UStoryGraphQuest::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Super::LoadPropertyFromXML(Properties);

	MainQuest = Properties["MainQuest"].Val == "true" ? true : false;
}

//UStoryGraphPlaceTrigger.................................................................................

UStoryGraphPlaceTrigger::UStoryGraphPlaceTrigger()
{
	ObjName = FText::FromString("Place Trigger" + FString::FromInt(PlaceTriggerNum++));
	ObjectType = EStoryObjectType::PlaceTrigger;
	ObjectState = (int)EPlaceTriggerStates::UnActive;
	DependedNodes.Add(ENodeType::AddMessageBranch);
}

void UStoryGraphPlaceTrigger::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("EPlaceTriggerStates"))
	{
		States.Add(GetEnumValueAsString<EPlaceTriggerStates>("EPlaceTriggerStates", (EPlaceTriggerStates)i++));
	}
}

void UStoryGraphPlaceTrigger::GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects)
{
	TExtractSceneObjects<IStorySceneObject, APlaceTrigger_StoryGraph>(SceneObjects, ScenTriggers, PlaceTriggerPointers);
}

void UStoryGraphPlaceTrigger::GetSceneObjects(TArray<AActor*>& SceneObjects)
{
	TExtractSceneObjects<AActor, APlaceTrigger_StoryGraph>(SceneObjects, ScenTriggers, PlaceTriggerPointers);
}

void UStoryGraphPlaceTrigger::SetSceneObjectRealPointers()
{
	PlaceTriggerPointers.Empty();
	for (int i = 0; i < ScenTriggers.Num(); i++)
	{
		PlaceTriggerPointers.Add(ScenTriggers[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphPlaceTrigger::ClearSceneObjects()
{
	ScenTriggers.Empty();
}

void UStoryGraphPlaceTrigger::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	for (int i = 0; i < GraphNodes.Num(); i++)
	{
		if (Cast<UDialogStartNode>(GraphNodes[i]))
		{
			Objects.Add(GraphNodes[i]);
		}
	}
}

void UStoryGraphPlaceTrigger::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("DefaultAnswer", XMLProperty(DefaultAnswer.ToString())));
	Properties.insert(std::pair<FString, XMLProperty>("PlaceTriggerType",
	                                                 XMLProperty(
		                                                 GetEnumValueAsString<EPlaceTriggerType>(
			                                                 "EPlaceTriggerType", PlaceTriggerType))));
}

void UStoryGraphPlaceTrigger::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Super::LoadPropertyFromXML(Properties);

	DefaultAnswer = FText::FromString(Properties["DefaultAnswer"].Val);
	PlaceTriggerType = GetEnumValueFromString<EPlaceTriggerType
	>("EPlaceTriggerType", Properties["PlaceTriggerType"].Val);
}

//UStoryGraphDialogTrigger.................................................................................

UStoryGraphDialogTrigger::UStoryGraphDialogTrigger()
{
	ObjName = FText::FromString("Dialog Trigger" + FString::FromInt(DialogTriggerNum++));
	ObjectType = EStoryObjectType::DialogTrigger;

	DependedNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependedNodes.Add(ENodeType::SetDialogTrigger);
}

void UStoryGraphDialogTrigger::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("EDialogTriggerStates"))
	{
		States.Add(GetEnumValueAsString<EDialogTriggerStates>("EDialogTriggerStates", (EDialogTriggerStates)i++));
	}
}

//UStoryGraphInventoryItem.................................................................................

UStoryGraphInventoryItem::UStoryGraphInventoryItem()
{
	ObjName = FText::FromString("Inventory Item" + FString::FromInt(InventoryItemNum++));
	ObjectType = EStoryObjectType::InventoryItem;
	InventoryItemWithoutSceneObject = false;

	DependedNodes.Add(ENodeType::SetInventoryItemState);
	DependedNodes.Add(ENodeType::SetInventoryItemStateFromMessage);
}

void UStoryGraphInventoryItem::GetObjectStateAsString(TArray<FString>& States)
{
	States.Add("UnActive");
	States.Add("OnLevel");
	if (InventoryItemPhase.Num() == 0)
	{
		States.Add("InInventory");
	}
	else
	{
		for (int i = 0; i < InventoryItemPhase.Num(); i++)
		{
			States.Add(InventoryItemPhase[i].ToString());
		}
	}
}

void UStoryGraphInventoryItem::GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects)
{
	TExtractSceneObjects<IStorySceneObject, AInventoryItem_StoryGraph>(SceneObjects, SceneInventoryItems,
	                                                                   InventoryItemPointers);
}

void UStoryGraphInventoryItem::GetSceneObjects(TArray<AActor*>& SceneObjects)
{
	TExtractSceneObjects<AActor, AInventoryItem_StoryGraph>(SceneObjects, SceneInventoryItems, InventoryItemPointers);
}

void UStoryGraphInventoryItem::SetSceneObjectRealPointers()
{
	InventoryItemPointers.Empty();
	for (int i = 0; i < SceneInventoryItems.Num(); i++)
	{
		InventoryItemPointers.Add(SceneInventoryItems[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphInventoryItem::ClearSceneObjects()
{
	SceneInventoryItems.Empty();
}

void UStoryGraphInventoryItem::SetCurrentState(int NewState)
{
	if (NewState == 0)
	{
		SetSceneObjectActive(false);
	}
	else if (NewState > 0)
	{
		if (!IsSceneObjectActive)
		{
			SetSceneObjectActive(false);
		}
	}

	if (NewState == 1)
	{
		ObjectState = (int)EInventoryItemStates::OnLevel;
	}
	else if (NewState > 1)
	{
		ObjectState = (int)EInventoryItemStates::InInventory;
	}

	if (InventoryItemPhase.Num() > 0 && NewState > 1)
	{
		CurrentItemPhase = NewState - 2;
	}

	((UStoryGraph*)GetOuter())->RefreshExecutionTrees();
}

int UStoryGraphInventoryItem::GetCurrentState()
{
	if (!IsSceneObjectActive)
	{
		return 0;
	}

	if (ObjectState == (int)EInventoryItemStates::OnLevel)
	{
		return 1;
	}

	if (ObjectState == (int)EInventoryItemStates::InInventory)
	{
		if (InventoryItemPhase.Num() > 0)
		{
			return CurrentItemPhase + 2;
		}
		else
		{
			return 2;
		}
	}
	return 0;
}

void UStoryGraphInventoryItem::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);


	Properties.insert(std::pair<FString, XMLProperty>("InventoryItemWithoutSceneObject",
	                                                 XMLProperty(InventoryItemWithoutSceneObject ? "true" : "false")));
	Properties.insert(std::pair<FString, XMLProperty>("Arr_InventoryItemPhase", XMLProperty("")));

	XMLProperty& InventoryItemPhasePointer = Properties["Arr_InventoryItemPhase"];

	for (int i = 0; i < InventoryItemPhase.Num(); i++)
	{
		InventoryItemPhasePointer.Properties.insert(
			std::pair<FString, XMLProperty>(FString::FromInt(i),
			                                XMLProperty(InventoryItemPhase[i].ToString())));
	}
}

void UStoryGraphInventoryItem::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Super::LoadPropertyFromXML(Properties);

	InventoryItemWithoutSceneObject = Properties["InventoryItemWithoutSceneObject"].Val == "true" ? true : false;

	for (auto it = Properties["Arr_InventoryItemPhase"].Properties.begin(); it != Properties["Arr_InventoryItemPhase"]
	                                                                             .Properties.end(); ++it)
	{
		InventoryItemPhase.Add(FText::FromString(it->second.Val));
	}
}

//UStoryGraphOthers.................................................................................

UStoryGraphOthers::UStoryGraphOthers()
{
	ObjName = FText::FromString("Object" + FString::FromInt(OthersNum++));
	ObjectType = EStoryObjectType::Others;

	DependedNodes.RemoveSingle(ENodeType::GetStoryGraphObjectState);
}


void UStoryGraphOthers::GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects)
{
	TExtractSceneObjects<IStorySceneObject, AOtherActor_StoryGraph>(SceneObjects, SceneOtherObjects, OtherPointers);
}

void UStoryGraphOthers::GetSceneObjects(TArray<AActor*>& SceneObjects)
{
	TExtractSceneObjects<AActor, AOtherActor_StoryGraph>(SceneObjects, SceneOtherObjects, OtherPointers);
}

void UStoryGraphOthers::SetSceneObjectRealPointers()
{
	OtherPointers.Empty();
	for (int i = 0; i < SceneOtherObjects.Num(); i++)
	{
		OtherPointers.Add(SceneOtherObjects[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphOthers::ClearSceneObjects()
{
	SceneOtherObjects.Empty();
}
