// Copyright 2016 Dmitriy Pavlov

/* If You want add new object. You must:   function
1) add line in EStoryObjectType enum.
2) adjust UStoryGraphObject::GetClassFromStoryObjectType
3) If object has scen object adjust FStoryGraphEditorModule::StartupModule for check property panel
*/
#pragma once

#include "Object.h"
#include "ObjectrRecord.h"
#include <map>
#include "EngineMinimal.h"
#include "StoryGraphObject.generated.h"



class UEdGraph_StoryGraph;

UENUM(BlueprintType)
enum class EStoryObjectType : uint8
{
	Non,
	Quest,
	Character,
	PlaceTrigger,
	DialogTrigger,
	InventoryItem,
	Others
};

template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByIndex((int32)Value);
}

template<typename TEnum>
static FORCEINLINE TEnum GetEnumValueFromString(const FString& NameOfClass, const FString& ValueName)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *NameOfClass, true);
	if (!EnumPtr) return (TEnum)0;

	return (TEnum)EnumPtr->GetIndexByName(FName(*ValueName));
}


static FORCEINLINE int GetNumberEnums(const FString& Name)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return -1;
	}
	return enumPtr->NumEnums() - 1;
	
}



enum class ENodeType : uint8;

struct XMLProperty
{
	XMLProperty(FString Val_): Val(Val_){}
	XMLProperty() {}
	FString Val;
	std::map<FString, XMLProperty> Propertys;
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphObject : public UObject, public ISaveObject_StoryGraph
{
	GENERATED_BODY()
public:		

	UPROPERTY()
	EStoryObjectType ObjectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ObjName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Category;

	UPROPERTY()
	FString Comment;

	FString XMLID;

	static int CharNum;

	static int QuestNum;

	static int PlaceTriggerNum;

	static int DialogTriggerNum;

	static int OthersNum;

	static int InventoryItemNum;


public:

	UStoryGraphObject();

	static FString GetObjectTypeEnumAsString(EStoryObjectType);

	TArray<ENodeType> DependetNodes;

	virtual void GetObjectStateAsString(TArray<FString>& States) { }

	virtual void DoubleClick() {}

	static TSubclassOf<UStoryGraphObject> GetClassFromStoryObjectType(EStoryObjectType EnumValue);

	static FString GetObjectToolTip(EStoryObjectType EnumValue);

	virtual void SetCurentState(int NewState);

	virtual int GetCurentState() { return ObjectState; }

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys);

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys);

protected:
	UPROPERTY(BlueprintReadOnly, SaveGame)
		int32 ObjectState;


};

UCLASS(Abstract)

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphObjectWithScenObject : public UStoryGraphObject
{
	GENERATED_BODY()
public:
	UStoryGraphObjectWithScenObject();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame)
	bool IsScenObjectActive;

	UPROPERTY()
	bool RealPointersActive;

	UPROPERTY(SaveGame)
		TArray<FString> ObjectMesssageStack;

public:	
	
	virtual void SetScenObjectRealPointers() {}

	virtual void ClearScenObjects() {}

	virtual void GetScenObjects(TArray<class IStoryScenObject*>& ScenObjects) {}

	virtual void GetScenObjects(TArray<AActor*>& ScenObjects) {}

	virtual int GetScenObjectsNum() { return 0; }

	void InitializeObject();

	void SetActiveStateOfScenObjects();

	void SetScenObjectActive(bool Active);

	void SendMessageToScenObject(FString Message);

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;

	template<class ReturnType, class ObjectType>
	void TExstractScenObgects(TArray<ReturnType*>& ScenObjects, TArray<TAssetPtr<ObjectType>> ScenLazyPointerArray, TArray<ObjectType*> ScenPointerArray)
	{
		ScenObjects.Empty();
		if (RealPointersActive)
		{
			for (int i = 0; i < ScenPointerArray.Num(); i++)
			{
				if (ScenPointerArray[i])
				{
					ScenObjects.Add((ReturnType*)ScenPointerArray[i]);
				}
			}
		}
		else
		{
			for (int i = 0; i < ScenLazyPointerArray.Num(); i++)
			{
				
				if (ScenLazyPointerArray[i].Get())
				{
					ScenObjects.Add((ReturnType*)ScenLazyPointerArray[i].Get());
				}
			}
		}
		
	}
};

UENUM(BlueprintType)
enum class ECharecterStates : uint8
{
	Alive,
	Dead
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphCharecter : public UStoryGraphObjectWithScenObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<TAssetPtr<class ACharecter_StoryGraph>> ScenCharecters;

	UPROPERTY()
		FText DefaultAnswer;

	UPROPERTY()
	TArray<class ACharecter_StoryGraph*> ScenCharecterPointers;

	UPROPERTY()
		TArray<class UCustomNodeBase*> GarphNods;

public:
	UStoryGraphCharecter();
	
	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void GetScenObjects(TArray<class IStoryScenObject*>& ScenObjects) override;

	virtual void GetScenObjects(TArray<AActor*>& ScenObjects) override;

	virtual void SetScenObjectRealPointers() override;

	virtual void ClearScenObjects() override;

	virtual int GetScenObjectsNum() override { return ScenCharecters.Num(); }

	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;// ISaveObject_StoryGraph interface

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override; 

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
};

UENUM(BlueprintType)
enum class EQuestPhaseState : uint8
{
	UnActive,
	Active,
};

UCLASS(BlueprintType)
class UQuestPhase : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
		FText Decription;

	UPROPERTY(BlueprintReadOnly)
		TArray<AActor*> PhaseObjects;

	UPROPERTY(BlueprintReadOnly)
		EQuestPhaseState QuestPhaseState;

	UPROPERTY()
	class UStoryGraphQuest* pOwnedQuest;

public:
	UQuestPhase() : QuestPhaseState(EQuestPhaseState::UnActive) {}
};

UENUM(BlueprintType)
enum class EQuestStates : uint8
{
	UnActive,
	Active,
	Complite,
	Canceled
};

UCLASS(BlueprintType)

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphQuest : public UStoryGraphObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, SaveGame)
		TArray<UQuestPhase*> QuestPhase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool MainQuest;

	
	UStoryGraphQuest();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void SetCurentState(int NewState) override;

	void AddPhase(UQuestPhase* Phase);

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
};

UENUM(BlueprintType)
enum class EPlaceTriggerStates : uint8
{
	UnActive,
	Active
};

UENUM(BlueprintType)
enum class EPlaceTriggerType : uint8
{
	UnInteractive UMETA(ToolTip = "Objec don't show message when it turn up in Aim"),
	Interactive UMETA(ToolTip = "Objec show message when it turn up in Aim"),
	AdvanceInteractive UMETA(ToolTip = "Objec show any messages witch determinate in Message graph"),
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphPlaceTrigger : public UStoryGraphObjectWithScenObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<TAssetPtr<class APlaceTrigger_StoryGraph>> ScenTriggers;

	UPROPERTY()
	TArray<class APlaceTrigger_StoryGraph*> PlaceTriggerPointers;

/*If active, tigger will show message when it turn up in Aim*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		EPlaceTriggerType PlaceTriggerType;

	UPROPERTY()
		TArray<class UCustomNodeBase*> GarphNods;

	UPROPERTY()
		FText DefaultAnswer;
public:

	UStoryGraphPlaceTrigger();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;
	
	virtual void GetScenObjects(TArray<IStoryScenObject*>& ScenObjects) override;

	virtual void GetScenObjects(TArray<AActor*>& ScenObjects) override;

	virtual void SetScenObjectRealPointers() override;

	virtual void ClearScenObjects() override;

	virtual int GetScenObjectsNum() override { return ScenTriggers.Num(); }

	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;// ISaveObject_StoryGraph interface

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
};

UENUM(BlueprintType)
enum class EDialogTriggerStates : uint8
{
	UnActive,
	Active
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphDialogTrigger : public UStoryGraphObject
{
	GENERATED_BODY()

public:
	UStoryGraphDialogTrigger();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

};

UENUM(BlueprintType)
enum class EInventoryItemeStates : uint8
{
	OnLevel,
	InInventory
};

UCLASS(BlueprintType)

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphInventoryItem : public UStoryGraphObjectWithScenObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool InventoryItemWithoutScenObject;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<TAssetPtr<class AInventoryItem_StoryGraph>> ScenInventoryItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTexture2D* Icone;
	
	UPROPERTY()
		TArray<class AInventoryItem_StoryGraph*> InventoryItemPointers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FText> InventoryItemPhase;

	UPROPERTY(BlueprintReadOnly, SaveGame)
		int32 CurrentItemPhase;

public:

	UStoryGraphInventoryItem();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void GetScenObjects(TArray<IStoryScenObject*>& ScenObjects) override;

	virtual void GetScenObjects(TArray<AActor*>& ScenObjects) override;

	virtual void SetScenObjectRealPointers() override;

	virtual void ClearScenObjects() override;

	virtual int GetScenObjectsNum() override { return ScenInventoryItems.Num(); }
	
	virtual void SetCurentState(int NewState) override;

	virtual int GetCurentState() override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphOthers : public UStoryGraphObjectWithScenObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<TAssetPtr<class AOtherActor_StoryGraph>> ScenOtherObjects;

	UPROPERTY()
		TArray<class AOtherActor_StoryGraph*> OtherPointers;

	UStoryGraphOthers();

	virtual void GetScenObjects(TArray<IStoryScenObject*>& ScenObjects) override;

	virtual void GetScenObjects(TArray<AActor*>& ScenObjects) override;

	virtual void SetScenObjectRealPointers() override;

	virtual void ClearScenObjects() override;

	virtual int GetScenObjectsNum() override { return ScenOtherObjects.Num(); }
};

