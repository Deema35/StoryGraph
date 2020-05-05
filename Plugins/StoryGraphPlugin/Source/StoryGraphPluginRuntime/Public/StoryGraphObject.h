// Copyright 2016 Dmitriy Pavlov

/* If You want add new object. You must:   function
1) add line in EStoryObjectType enum.
2) adjust UStoryGraphObject::GetClassFromStoryObjectType
3) If object has scene object adjust FStoryGraphEditorModule::StartupModule for check property panel
*/
#pragma once

#include <map>
#include "EngineMinimal.h"
#include "SaveObject_StoryGraph.h"
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

template <typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByIndex((int32)Value);
}

template <typename TEnum>
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
	XMLProperty(FString Val_): Val(Val_)
	{
	}

	XMLProperty()
	{
	}

	FString Val;
	std::map<FString, XMLProperty> Properties;
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

	TArray<ENodeType> DependedNodes;

	virtual void GetObjectStateAsString(TArray<FString>& States)
	{
	}

	virtual void DoubleClick()
	{
	}

	static TSubclassOf<UStoryGraphObject> GetClassFromStoryObjectType(EStoryObjectType EnumValue);

	static FString GetObjectToolTip(EStoryObjectType EnumValue);

	virtual void SetCurrentState(int NewState);

	virtual int GetCurrentState() { return ObjectState; }

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties);

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties);

protected:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	int32 ObjectState;
};

UCLASS(Abstract)

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphObjectWithSceneObject : public UStoryGraphObject
{
	GENERATED_BODY()
public:
	UStoryGraphObjectWithSceneObject();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame)
	bool IsSceneObjectActive;

	UPROPERTY()
	bool RealPointersActive;

	UPROPERTY(SaveGame)
	TArray<FString> ObjectMessageStack;

public:

	virtual void SetSceneObjectRealPointers()
	{
	}

	virtual void ClearSceneObjects()
	{
	}

	virtual void GetSceneObjects(TArray<class IStorySceneObject*>& SceneObjects)
	{
	}

	virtual void GetSceneObjects(TArray<AActor*>& SceneObjects)
	{
	}

	virtual int GetSceneObjectsNum() { return 0; }

	void InitializeObject();

	void SetActiveStateOfSceneObjects();

	void SetSceneObjectActive(bool Active);

	void SendMessageToSceneObject(FString Message);

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties) override;

	template <class ReturnType, class ObjectType>
	void TExtractSceneObjects(TArray<ReturnType*>& SceneObjects, TArray<TAssetPtr<ObjectType>> SceneLazyPointerArray,
	                          TArray<ObjectType*> ScenePointerArray)
	{
		SceneObjects.Empty();
		if (RealPointersActive)
		{
			for (int i = 0; i < ScenePointerArray.Num(); i++)
			{
				if (ScenePointerArray[i])
				{
					SceneObjects.Add((ReturnType*)ScenePointerArray[i]);
				}
			}
		}
		else
		{
			for (int i = 0; i < SceneLazyPointerArray.Num(); i++)
			{
				if (SceneLazyPointerArray[i].Get())
				{
					SceneObjects.Add((ReturnType*)SceneLazyPointerArray[i].Get());
				}
			}
		}
	}
};
UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphCharacter : public UStoryGraphObjectWithSceneObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TAssetPtr<class ACharacter_StoryGraph>> SceneCharacters;

	UPROPERTY()
	FText DefaultAnswer;

	UPROPERTY()
	TArray<class ACharacter_StoryGraph*> SceneCharacterPointers;

	UPROPERTY()
	TArray<class UCustomNodeBase*> GraphNodes;

public:
	UStoryGraphCharacter();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void GetSceneObjects(TArray<class IStorySceneObject*>& SceneObjects) override;

	virtual void GetSceneObjects(TArray<AActor*>& SceneObjects) override;

	virtual void SetSceneObjectRealPointers() override;

	virtual void ClearSceneObjects() override;

	virtual int GetSceneObjectsNum() override { return SceneCharacters.Num(); }

	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;
	// ISaveObject_StoryGraph interface

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties) override;
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
	FText Description;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> PhaseObjects;

	UPROPERTY(BlueprintReadOnly)
	EQuestPhaseState QuestPhaseState;

	UPROPERTY()
	class UStoryGraphQuest* pOwnedQuest;

public:
	UQuestPhase() : QuestPhaseState(EQuestPhaseState::UnActive)
	{
	}
};

UENUM(BlueprintType)
enum class EQuestStates : uint8
{
	UnActive,
	Active,
	Complete,
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

	virtual void SetCurrentState(int NewState) override;

	void AddPhase(UQuestPhase* Phase);

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties) override;
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
	UnInteractive UMETA(ToolTip = "Object don't show message when it turn up in Aim"),
	Interactive UMETA(ToolTip = "Object show message when it turn up in Aim"),
	AdvanceInteractive UMETA(ToolTip = "Object show any messages witch determinate in Message graph"),
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphPlaceTrigger : public UStoryGraphObjectWithSceneObject
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
	TArray<class UCustomNodeBase*> GraphNodes;

	UPROPERTY()
	FText DefaultAnswer;
public:

	UStoryGraphPlaceTrigger();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects) override;

	virtual void GetSceneObjects(TArray<AActor*>& SceneObjects) override;

	virtual void SetSceneObjectRealPointers() override;

	virtual void ClearSceneObjects() override;

	virtual int GetSceneObjectsNum() override { return ScenTriggers.Num(); }

	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) override;
	// ISaveObject_StoryGraph interface

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties) override;
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
enum class EInventoryItemStates : uint8
{
	OnLevel,
	InInventory
};

UCLASS(BlueprintType)

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphInventoryItem : public UStoryGraphObjectWithSceneObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool InventoryItemWithoutSceneObject;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TAssetPtr<class AInventoryItem_StoryGraph>> SceneInventoryItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY()
	TArray<class AInventoryItem_StoryGraph*> InventoryItemPointers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FText> InventoryItemPhase;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	int32 CurrentItemPhase;

public:

	UStoryGraphInventoryItem();

	virtual void GetObjectStateAsString(TArray<FString>& States) override;

	virtual void GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects) override;

	virtual void GetSceneObjects(TArray<AActor*>& SceneObjects) override;

	virtual void SetSceneObjectRealPointers() override;

	virtual void ClearSceneObjects() override;

	virtual int GetSceneObjectsNum() override { return SceneInventoryItems.Num(); }

	virtual void SetCurrentState(int NewState) override;

	virtual int GetCurrentState() override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties) override;

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties) override;
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryGraphOthers : public UStoryGraphObjectWithSceneObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TAssetPtr<class AOtherActor_StoryGraph>> SceneOtherObjects;

	UPROPERTY()
	TArray<class AOtherActor_StoryGraph*> OtherPointers;

	UStoryGraphOthers();

	virtual void GetSceneObjects(TArray<IStorySceneObject*>& SceneObjects) override;

	virtual void GetSceneObjects(TArray<AActor*>& SceneObjects) override;

	virtual void SetSceneObjectRealPointers() override;

	virtual void ClearSceneObjects() override;

	virtual int GetSceneObjectsNum() override { return SceneOtherObjects.Num(); }
};
