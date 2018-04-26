// Copyright 2016 Dmitriy Pavlov

#include "StoryGraphObject.h"
#include "StoryGraph.h"
#include "StoryScenObject.h"
#include "HUD_StoryGraph.h"
#include "CustomNods.h"
#include "StoryGraphWiget.h"
#if WITH_EDITORONLY_DATA
#include "AssetEditorManager.h"
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

		return UStoryGraphCharecter::StaticClass();

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

void UStoryGraphObject::SetCurentState(int NewState)
{

	ObjectState = NewState;
	((UStoryGraph*)GetOuter())->RefreshExecutionTrees();
}

void UStoryGraphObject::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Propertys.clear();

	Propertys.insert(std::pair<FString, XMLProperty>("ObjName", XMLProperty(ObjName.ToString())));
	Propertys.insert(std::pair<FString, XMLProperty>("ObjectType", XMLProperty(GetEnumValueAsString<EStoryObjectType>("EStoryObjectType", ObjectType))));
	Propertys.insert(std::pair<FString, XMLProperty>("Category", XMLProperty( Category)));
	Propertys.insert(std::pair<FString, XMLProperty>("Comment", XMLProperty(Comment)));
	

}

void UStoryGraphObject::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	ObjName = FText::FromString(Propertys["ObjName"].Val);
	Category = Propertys["Category"].Val;
	Comment = Propertys["Comment"].Val;
}
//UStoryGraphObjectWithScenObject.........................................................

UStoryGraphObjectWithScenObject::UStoryGraphObjectWithScenObject()
{
	IsScenObjectActive = true;
	DependetNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependetNodes.Add(ENodeType::SetScenObjectActive);
	DependetNodes.Add(ENodeType::SendMessageCsen);
	DependetNodes.Add(ENodeType::AddTargetObjectToPhase);
	
}

void UStoryGraphObjectWithScenObject::InitializeObject()
{
	SetActiveStateOfScenObjects();
	TArray<IStoryScenObject*> ScenObjects;
	GetScenObjects(ScenObjects);
	for (int i = 0; i < ObjectMesssageStack.Num(); i++)
	{
		
		for (int j = 0; j < ScenObjects.Num(); j++)
		{
			ScenObjects[j]->SendMessageToScenObject(ObjectMesssageStack[i]);
		}
	}
}

void UStoryGraphObjectWithScenObject::SetActiveStateOfScenObjects()
{
	TArray<IStoryScenObject*> ScenObjects;

	GetScenObjects(ScenObjects);
	for (int i = 0; i < ScenObjects.Num(); i++)
	{
		ScenObjects[i]->RefreshScenObjectActive();
	}
}

void UStoryGraphObjectWithScenObject::SetScenObjectActive(bool Active)
{
	IsScenObjectActive = Active;
	SetActiveStateOfScenObjects();
}

void UStoryGraphObjectWithScenObject::SendMessageToScenObject(FString Message)
{
	TArray<IStoryScenObject*> ScenObjects;
	GetScenObjects(ScenObjects);
	for (int i = 0; i < ScenObjects.Num(); i++)
	{
		ScenObjects[i]->SendMessageToScenObject(Message);
	}

	ObjectMesssageStack.Add(Message);
}

void UStoryGraphObjectWithScenObject::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Super::GetXMLSavingProperty(Propertys);
	Propertys.insert(std::pair<FString, XMLProperty>("IsScenObjectActive", XMLProperty(IsScenObjectActive ? "true" : "false" )));
}

void UStoryGraphObjectWithScenObject::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	Super::LoadPropertyFromXML(Propertys);
	IsScenObjectActive = Propertys["IsScenObjectActive"].Val == "true" ? true : false;
	
}
//UStoryGraphCharecter.................................................................................

UStoryGraphCharecter::UStoryGraphCharecter()
{
	ObjName = FText::FromString("New Character" + FString::FromInt(CharNum++));
	ObjectType = EStoryObjectType::Character;
	DependetNodes.Add(ENodeType::AddDialog);
	
}

void UStoryGraphCharecter::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("ECharecterStates"))
	{
		States.Add(GetEnumValueAsString<ECharecterStates>("ECharecterStates", (ECharecterStates)i++));
	}
	
}

void UStoryGraphCharecter::GetScenObjects(TArray<IStoryScenObject*>& ScenObjects)
{
	TExstractScenObgects<IStoryScenObject, ACharecter_StoryGraph>(ScenObjects, ScenCharecters, ScenCharecterPointers);
}

void UStoryGraphCharecter::GetScenObjects(TArray<AActor*>& ScenObjects)
{
	TExstractScenObgects<AActor, ACharecter_StoryGraph>(ScenObjects, ScenCharecters, ScenCharecterPointers);
}

void UStoryGraphCharecter::SetScenObjectRealPointers()
{
	ScenCharecterPointers.Empty();
	for (int i = 0; i < ScenCharecters.Num(); i++)
	{
		ScenCharecterPointers.Add(ScenCharecters[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphCharecter::ClearScenObjects()
{
	ScenCharecters.Empty();
}

void UStoryGraphCharecter::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	for (int i = 0; i < GarphNods.Num(); i++)
	{
		if (Cast<UDialogStartNode>(GarphNods[i]))
		{
			Objects.Add(GarphNods[i]);
		}

	}

}

void UStoryGraphCharecter::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Super::GetXMLSavingProperty(Propertys);
	Propertys.insert(std::pair<FString, XMLProperty>("DefaultAnswer", XMLProperty(DefaultAnswer.ToString())));
}

void UStoryGraphCharecter::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	Super::LoadPropertyFromXML(Propertys);
	DefaultAnswer = FText::FromString(Propertys["DefaultAnswer"].Val);

}

//UStoryGraphQuest.................................................................................

UStoryGraphQuest::UStoryGraphQuest()
{
	ObjName = FText::FromString("New Quest" + FString::FromInt(QuestNum++));
	ObjectType = EStoryObjectType::Quest;
	MainQuest = true;
	
	DependetNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependetNodes.Add(ENodeType::QuestStart);
	DependetNodes.Add(ENodeType::CancelQuest);
}

void UStoryGraphQuest::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("EQuestStates"))
	{
		States.Add(GetEnumValueAsString<EQuestStates>("EQuestStates", (EQuestStates)i++));
	}

}


void UStoryGraphQuest::SetCurentState(int NewState)
{
	FText Mesg;

	ObjectState = NewState;

	switch ((EQuestStates)NewState)
	{
	case EQuestStates::Active:

		Mesg = FText::Format(NSLOCTEXT("StoryGraph", "Quest active", "New quest {0}"), ObjName);

		break;

	case EQuestStates::Canceled:

		Mesg = FText::Format(NSLOCTEXT("StoryGraph", "Quest cancel", "Quest {0} cancel"), ObjName);

		break;

	case EQuestStates::Complite:

		Mesg = FText::Format(NSLOCTEXT("StoryGraph", "Quest complite", "Quest {0} complite"), ObjName);

		break;
	}
	
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(((UStoryGraph*)GetOuter())->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		if (HUD->GameScreen)
		{
			HUD->GameScreen->AddMessageOnScreen(Mesg, 5);
		}
	}
	
	
	
	((UStoryGraph*)GetOuter())->QuestStateWasChange = true;
}

void UStoryGraphQuest::AddPhase(UQuestPhase* Phase)
{
	
	if (QuestPhase.Num() == 0)
	{
		SetCurentState((int)EQuestStates::Active);
	}
	else
	{
		
			if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>((((UStoryGraph*)GetOuter())->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD())))
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

void UStoryGraphQuest::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Super::GetXMLSavingProperty(Propertys);

	Propertys.insert(std::pair<FString, XMLProperty>("MainQuest", XMLProperty(MainQuest  ? "true" : "false")));
}

void UStoryGraphQuest::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	Super::LoadPropertyFromXML(Propertys);

	MainQuest = Propertys["MainQuest"].Val == "true" ? true : false;

}

//UStoryGraphPlaceTrigger.................................................................................

UStoryGraphPlaceTrigger::UStoryGraphPlaceTrigger()
{
	ObjName = FText::FromString("Place Trigger" + FString::FromInt(PlaceTriggerNum++));
	ObjectType = EStoryObjectType::PlaceTrigger;
	ObjectState = (int)EPlaceTriggerStates::UnActive;
	DependetNodes.Add(ENodeType::AddMessageBranch);
	
}

void UStoryGraphPlaceTrigger::GetObjectStateAsString(TArray<FString>& States)
{
	int i = 0;
	while (i < GetNumberEnums("EPlaceTriggerStates"))
	{
		States.Add(GetEnumValueAsString<EPlaceTriggerStates>("EPlaceTriggerStates", (EPlaceTriggerStates)i++));
	}

}

void UStoryGraphPlaceTrigger::GetScenObjects(TArray<IStoryScenObject*>& ScenObjects)
{
	TExstractScenObgects<IStoryScenObject, APlaceTrigger_StoryGraph>(ScenObjects, ScenTriggers, PlaceTriggerPointers);
	
}

void UStoryGraphPlaceTrigger::GetScenObjects(TArray<AActor*>& ScenObjects)
{
	TExstractScenObgects<AActor, APlaceTrigger_StoryGraph>(ScenObjects, ScenTriggers, PlaceTriggerPointers);

}

void UStoryGraphPlaceTrigger::SetScenObjectRealPointers()
{
	PlaceTriggerPointers.Empty();
	for (int i = 0; i < ScenTriggers.Num(); i++)
	{
		PlaceTriggerPointers.Add(ScenTriggers[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphPlaceTrigger::ClearScenObjects()
{
	ScenTriggers.Empty();
}

void UStoryGraphPlaceTrigger::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	for (int i = 0; i < GarphNods.Num(); i++)
	{
		if (Cast<UDialogStartNode>(GarphNods[i]))
		{
			Objects.Add(GarphNods[i]);
		}

	}

}

void UStoryGraphPlaceTrigger::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Super::GetXMLSavingProperty(Propertys);

	Propertys.insert(std::pair<FString, XMLProperty>("DefaultAnswer", XMLProperty(DefaultAnswer.ToString())));
	Propertys.insert(std::pair<FString, XMLProperty>("PlaceTriggerType", XMLProperty(GetEnumValueAsString<EPlaceTriggerType>("EPlaceTriggerType", PlaceTriggerType))));
}

void UStoryGraphPlaceTrigger::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	Super::LoadPropertyFromXML(Propertys);

	DefaultAnswer = FText::FromString(Propertys["DefaultAnswer"].Val);
	PlaceTriggerType = GetEnumValueFromString<EPlaceTriggerType>("EPlaceTriggerType", Propertys["PlaceTriggerType"].Val);

}

//UStoryGraphDialogTrigger.................................................................................

UStoryGraphDialogTrigger::UStoryGraphDialogTrigger()
{
	ObjName = FText::FromString("Dialog Trigger" + FString::FromInt(DialogTriggerNum++));
	ObjectType = EStoryObjectType::DialogTrigger;

	DependetNodes.Add(ENodeType::GetStoryGraphObjectState);
	DependetNodes.Add(ENodeType::SetDialogTrigger);
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
	InventoryItemWithoutScenObject = false;

	DependetNodes.Add(ENodeType::SetInventoryItemState);
	DependetNodes.Add(ENodeType::SetInventoryItemStateFromMessage);
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

void UStoryGraphInventoryItem::GetScenObjects(TArray<IStoryScenObject*>& ScenObjects)
{
	TExstractScenObgects<IStoryScenObject, AInventoryItem_StoryGraph>(ScenObjects, ScenInventoryItems, InventoryItemPointers);
}

void UStoryGraphInventoryItem::GetScenObjects(TArray<AActor*>& ScenObjects)
{
	TExstractScenObgects<AActor, AInventoryItem_StoryGraph>(ScenObjects, ScenInventoryItems, InventoryItemPointers);
}

void UStoryGraphInventoryItem::SetScenObjectRealPointers()
{
	InventoryItemPointers.Empty();
	for (int i = 0; i < ScenInventoryItems.Num(); i++)
	{
		InventoryItemPointers.Add(ScenInventoryItems[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphInventoryItem::ClearScenObjects()
{
	ScenInventoryItems.Empty();
}

void UStoryGraphInventoryItem::SetCurentState(int NewState)
{
	if (NewState == 0)
	{
		SetScenObjectActive(false);
	}
	else if (NewState > 0)
	{
		if (!IsScenObjectActive)
		{
			SetScenObjectActive(false);
		}
	}

	if (NewState == 1)
	{
		ObjectState = (int)EInventoryItemeStates::OnLevel;
		
	}
	else if (NewState > 1)
	{
		ObjectState = (int)EInventoryItemeStates::InInventory;
	}

	if (InventoryItemPhase.Num() > 0 && NewState > 1)
	{
		CurrentItemPhase = NewState - 2;
	}

	((UStoryGraph*)GetOuter())->RefreshExecutionTrees();
}

int UStoryGraphInventoryItem::GetCurentState()
{
	if (!IsScenObjectActive)
	{
		return 0;
	}
	
	if (ObjectState == (int)EInventoryItemeStates::OnLevel)
	{
		return 1;
	}
	
	if (ObjectState == (int)EInventoryItemeStates::InInventory)
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

void UStoryGraphInventoryItem::GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys)
{
	Super::GetXMLSavingProperty(Propertys);

	

	Propertys.insert(std::pair<FString, XMLProperty>("InventoryItemWithoutScenObject", XMLProperty(InventoryItemWithoutScenObject ? "true" : "false")));
	Propertys.insert(std::pair<FString, XMLProperty>("Arr_InventoryItemPhase", XMLProperty("")));

	XMLProperty& InventoryItemPhasePointer = Propertys["Arr_InventoryItemPhase"];

	for (int i = 0; i < InventoryItemPhase.Num(); i++)
	{
		InventoryItemPhasePointer.Propertys.insert(std::pair<FString, XMLProperty>(FString::FromInt(i), XMLProperty( InventoryItemPhase[i].ToString())));
	}
}

void UStoryGraphInventoryItem::LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys)
{
	Super::LoadPropertyFromXML(Propertys);

	InventoryItemWithoutScenObject = Propertys["InventoryItemWithoutScenObject"].Val == "true" ? true : false;

	for (auto it = Propertys["Arr_InventoryItemPhase"].Propertys.begin(); it != Propertys["Arr_InventoryItemPhase"].Propertys.end(); ++it)
	{
		
		InventoryItemPhase.Add(FText::FromString(it->second.Val));
	}
}

//UStoryGraphOthers.................................................................................

UStoryGraphOthers::UStoryGraphOthers()
{
	ObjName = FText::FromString("Object" + FString::FromInt(OthersNum++));
	ObjectType = EStoryObjectType::Others;

	DependetNodes.RemoveSingle(ENodeType::GetStoryGraphObjectState);
}


void UStoryGraphOthers::GetScenObjects(TArray<IStoryScenObject*>& ScenObjects)
{
	TExstractScenObgects<IStoryScenObject, AOtherActor_StoryGraph>(ScenObjects, ScenOtherObjects, OtherPointers);
}

void UStoryGraphOthers::GetScenObjects(TArray<AActor*>& ScenObjects)
{
	TExstractScenObgects<AActor, AOtherActor_StoryGraph>(ScenObjects, ScenOtherObjects, OtherPointers);
}

void UStoryGraphOthers::SetScenObjectRealPointers()
{
	OtherPointers.Empty();
	for (int i = 0; i < ScenOtherObjects.Num(); i++)
	{
		OtherPointers.Add(ScenOtherObjects[i].Get());
	}

	RealPointersActive = true;
}

void UStoryGraphOthers::ClearScenObjects()
{
	ScenOtherObjects.Empty();
}