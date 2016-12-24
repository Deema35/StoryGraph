// Copyright 2016 Dmitriy Pavlov
#include "StoryGraphPluginRuntime.h"
#include "StoryGraphObject.h"
#include "StoryGraph.h"
#include "StoryScenObject.h"
#include "HUD_StoryGraph.h"
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

	return EnumPtr->GetEnumName((int)EnumValue);
}

void UStoryGraphObject::SetCurentState(int NewState)
{

	ObjectState = NewState;
	((UStoryGraph*)GetOuter())->RefreshExecutionTrees();
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
	if (((UStoryGraph*)GetOuter())->GameScreen)
	{
		((UStoryGraph*)GetOuter())->GameScreen->AddMessageOnScreen(Mesg, 5);
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
		FText Mesg = FText::Format(NSLOCTEXT("StoryGraph", "AddQuestPhaseMessage2", "Quest {0} changed"), ObjName);
		if (((UStoryGraph*)GetOuter())->GameScreen)
		{
			((UStoryGraph*)GetOuter())->GameScreen->AddMessageOnScreen(Mesg, 5);
		}
	}

	QuestPhase.Add(Phase);
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