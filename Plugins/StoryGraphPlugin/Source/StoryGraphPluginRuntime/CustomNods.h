// Copyright 2016 Dmitriy Pavlov

/* If You want add new node. You must:
1) add line in ENodeType enum.
2) adjust UCustomNodeBase::GetIncertNodeType.
3) adjust UCustomNodeBase::GetActionNameFromNodeType.
4) adjust UCustomNodeBase::GetClassFromNodeType.
5) Add new SGraphNode object if your need new node UI (and adjust FGraphPanelNodeFactory_StoryGraph::CreateNode).
6) If node is dependet type, you must add member in UStoryGraphObject::DependetNodes Array (from file StoryGraphObject.h).
*/

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "StoryGraphObject.h"

#include "CustomNods.generated.h"



UENUM(BlueprintType)
enum class EPinDataTypes : uint8
{
	PinType_Vertical,
	PinType_Horizontal
};

UENUM(BlueprintType)
enum class ECharDialogOwner : uint8
{
	NotDefine,
	Character,
	NPC
};

UENUM(BlueprintType)
enum class ENodeType : uint8
{
	PrintString,
	DialogStart,
	DialogNode,
	DialogEnd,
	DialogExit,
	SetScenObjectActive,
	GetStoryGraphObjectState,
	SetDialogTrigger,
	AddDialog,
	CancelQuest,
	QuestStart,
	AddQuestPhase,
	EndGame,
	SendMessageCsen,
	AddScreenMessage,
	AddTargetObjectToPhase,
	ActivateTrigger,
	MessageStart,
	Message,
	MessageExit,
	MessageEnd,
	SetInventoryItemState,
	SetInventoryItemStateFromMessage,
	PrintQuestPhaseOnScreen,
	AddMessageBranch,
	AddDialogFromDialog,
	SendMessagToLevelBlueprint
};

UENUM()
enum class EIncertNodeType
{
	NotDefine,
	StoryGraphStandalone,
	DialogGraphStandalone,
	MessageGraphStandalone,
	StoryGraphDependent,
	DialogGraphDependent,
	MessageGraphDependent
};

enum class EPerformNodeResult
{
	Sucssed,
	Fail,
	NodeAlreadyPerfomed,
	NodeHaveDialogs
};


class UStoryGraphObject;
class UStoryGraph;

USTRUCT()
struct FStoryGraphPin
{
	GENERATED_BODY()
	FStoryGraphPin() {}
	FStoryGraphPin(EEdGraphPinDirection Direction_ , EPinDataTypes PinDataType_) : Direction((int)Direction_), PinDataType((int)PinDataType_) {}
	UPROPERTY()
	int32 Direction;
	UPROPERTY()
	int32 PinDataType;
	UPROPERTY()
	TArray<UCustomNodeBase*> Links;
	int XMLID;
};



UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UCustomNodeBase : public UObject 
{
	GENERATED_BODY()
	DECLARE_DELEGATE(FPropertyUpdateDelegate);
	DECLARE_DELEGATE(FNodeUpdateDelegate);
	DECLARE_DELEGATE_OneParam(FCreatePinDelegate, FStoryGraphPin);
	DECLARE_DELEGATE_OneParam(FRemovePinDelegate, int32);
	DECLARE_DELEGATE_OneParam(FBreakPinDelegate, int32);
public:
	
	UPROPERTY()
	FString Comment;

	UPROPERTY()
	UStoryGraphObject* pGraphObject = nullptr;

	ENodeType NodeType;

	UPROPERTY()
	UStoryGraph* pStoryGraph = nullptr;

	UPROPERTY()
	TArray<FStoryGraphPin> NodePins;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
		FLinearColor NodeColor;

	FString XMLID;
#endif //WITH_EDITORONLY_DATA
	FNodeUpdateDelegate NodeUpdateDelegate; // Update Node UI

	FPropertyUpdateDelegate ProprtyUpdateDelegate; //Update detail panel
	
	FCreatePinDelegate CreatePinDelegate;
	
	FRemovePinDelegate RemovePinDelegate;
	
	FBreakPinDelegate BreakPinDelegate;
public:

	UCustomNodeBase() : pGraphObject(NULL), pStoryGraph(NULL)
#if WITH_EDITORONLY_DATA
		,NodeColor(FLinearColor(0.08f, 0.08f, 0.08f))
#endif //WITH_EDITORONLY_DATA
	{}


	void GetChildNodes(TArray<UCustomNodeBase*>& ChildNodes, EPinDataTypes OutPinType);

	UCustomNodeBase* GetFistChildNode();

	void GetInputNodes(TArray<UCustomNodeBase*>& InputNodes, EPinDataTypes OutPinType);

	static FString GetPinDataTypeEnumAsString(EPinDataTypes EnumValue);

	static FString GetActionNameFromNodeType(ENodeType NodeType);

	static FString GetToolTipFromNodeType(ENodeType NodeType);

	static FLinearColor GetCollorFromNodeType(ENodeType NodeType, int CollorNumber = 0);

	static EIncertNodeType GetIncertNodeType(ENodeType NodeType); //If node Dependent you must add this node to UStoryGraphObject::DependetNodes appropriate obgect

	static TSubclassOf<UCustomNodeBase> GetClassFromNodeType(ENodeType NodeType);

	static FName GetIconNameFromNodeType(ENodeType NodeType);

	virtual EPerformNodeResult PerformNode();

	virtual FText GetNodeTitle() const;
#if WITH_EDITORONLY_DATA
	virtual void InitNode(class UStoryGraphObject* pGraphObject_);

	virtual void RefreshCollor() { NodeColor = UCustomNodeBase::GetCollorFromNodeType(NodeType); }

	virtual void PinConnectionListChanged(FStoryGraphPin* Pin) {}


	void CreatePin(FStoryGraphPin NewPin);

	void RemovePin(int32 PinNumber);

	void UpdateGraphNode();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys); 

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys);

	void DelitLinkToNode(UCustomNodeBase* NodeLink);
#endif //WITH_EDITORONLY_DATA
};

UCLASS(BlueprintType)

class STORYGRAPHPLUGINRUNTIME_API UDialogObject : public UObject
{

	GENERATED_BODY()
public:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Dialog;

	UPROPERTY()
	class UCustomNodeBase* DialogNode;

	UPROPERTY()
	int32 CurrentDialogPin;

	
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UDialogNodeBase : public UCustomNodeBase
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TArray<UDialogObject*> Dialogs;

	UPROPERTY()
		UDialogObject* TopDialog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ECharDialogOwner DialogOwner;

	static FText GetDialogName(FText Dialog);

	virtual bool GetChildNode(const UDialogObject* CurrentDialog, UDialogNodeBase*& NextNode);
#if WITH_EDITORONLY_DATA
	virtual void RefreshDialogOwner();

	virtual void PinConnectionListChanged(FStoryGraphPin* Pin) override;

	void AddDialog();

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override; 

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;

	
#endif //WITH_EDITORONLY_DATA
};

UCLASS()

class STORYGRAPHPLUGINRUNTIME_API UStoryVerticalNodeBase : public UCustomNodeBase //Base class for nods witch have vertical pins
{
	GENERATED_BODY()
public:

	UStoryVerticalNodeBase() : bPerformNode(false), pQuestOwner(NULL), pQuestPhase(NULL){}

	UPROPERTY(Transient, SaveGame)
	bool bPerformNode;

	UPROPERTY()
	UStoryGraphQuest* pQuestOwner;

	UPROPERTY()
	UQuestPhase* pQuestPhase;

	void ResetUnPerformBrunch();

	virtual EPerformNodeResult PerformNode() override;
#if WITH_EDITORONLY_DATA
	virtual void RefreshQuestOwner();

	virtual void PinConnectionListChanged(FStoryGraphPin* Pin) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UStoryHorizontalNodeBase : public UCustomNodeBase //Base class for nods witch have horizontal pins
{
	GENERATED_BODY()
public:

	UPROPERTY()
	UQuestPhase* pQuestPhase;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
		FString OwnedQuestPhase;
#endif //WITH_EDITORONLY_DATA

public:
	virtual EPerformNodeResult PerformNode() override;
#if WITH_EDITORONLY_DATA
	void RefreshQuestPhase();

	virtual void PinConnectionListChanged(FStoryGraphPin*  Pin) override;
#endif //WITH_EDITORONLY_DATA
};


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UAddQuestPhaseNode : public UStoryVerticalNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
		UQuestPhase* QuestPhaseToAdd;
	/*If quest phase emty, it don't add to jurnal and it radar objects don't add to radar, when it active.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool IsEmpty;

	UAddQuestPhaseNode();
	
	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void InitNode(class UStoryGraphObject* pGraphObject_) override;
	void  SetQuestPhase(FText NewQuestPhase);

	virtual void RefreshCollor() override;

	virtual void PinConnectionListChanged(FStoryGraphPin*  Pin) override;

	virtual void RefreshQuestOwner() override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA

};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UDialogStartNode : public UDialogNodeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		bool IsActive;
	
	/*0 - hight priority*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 DialogPriority;

public:
	UDialogStartNode();

	virtual EPerformNodeResult PerformNode() override { return EPerformNodeResult::Sucssed; }
#if WITH_EDITORONLY_DATA
	virtual void InitNode(class UStoryGraphObject* pGraphObject_) override;

	void SetNewDialog(FText NewDialog);

	virtual void RefreshCollor() override;

	virtual void RefreshDialogOwner() override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UDialogNode : public UDialogNodeBase
{
	GENERATED_BODY()

public:
	
	UDialogNode();

	virtual EPerformNodeResult PerformNode() override { return EPerformNodeResult::NodeHaveDialogs; }
#if WITH_EDITORONLY_DATA
	virtual void InitNode(class UStoryGraphObject* pGraphObject_) override;

	virtual void RefreshDialogOwner() override;

	virtual void RefreshCollor() override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UDialogEndNode : public UDialogNodeBase
{
	GENERATED_BODY()
public:
	UDialogEndNode();

	virtual EPerformNodeResult PerformNode() override;
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UDialogExitNode : public UDialogNodeBase
{
	GENERATED_BODY()
public:
	UDialogExitNode();

	virtual EPerformNodeResult PerformNode() override;
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  USetDialogTriggerNode : public UDialogNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		EDialogTriggerStates TriggerState;

public:
	USetDialogTriggerNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
	
};


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  USetScenObjectActiveNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsActive;

public:

	USetScenObjectActiveNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UGetStoryGraphObjectStateNode : public UStoryVerticalNodeBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
		int32 WantedObjectState;


	UGetStoryGraphObjectStateNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

	void SetWantedObjectState(int WantedState_);

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA

};



UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UAddDialogNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UDialogStartNode* SelectedDialog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Activate;

public:
	UAddDialogNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;
#if WITH_EDITORONLY_DATA
	void SetCurentDialog(UDialogStartNode* SelectedDialog_);

	virtual void RefreshCollor() override;

	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UAddDialogFromDialogNode : public UDialogNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UDialogStartNode* SelectedDialog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Activate;

public:
	UAddDialogFromDialogNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

	void SetCurentDialog(UDialogStartNode* SelectedDialog_);

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};



UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UQuestStartNode : public UStoryVerticalNodeBase
{
	GENERATED_BODY()

public:
	UQuestStartNode();

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void InitNode(class UStoryGraphObject* pGraphObject_) override;

	virtual void RefreshQuestOwner() override;

	virtual void RefreshCollor() override;

#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UCancelQuestNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()

public:
	
	UCancelQuestNode();

	virtual EPerformNodeResult PerformNode() override;
		
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UEndGameNode : public UStoryVerticalNodeBase
{
	GENERATED_BODY()

public:
	UEndGameNode();

	virtual EPerformNodeResult PerformNode() override;

#if WITH_EDITORONLY_DATA
	virtual void RefreshCollor() override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  USendMessageNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Message;

public:
	USendMessageNode();

	virtual EPerformNodeResult PerformNode() override;
	
	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys);  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UPrintStringNode : public UStoryVerticalNodeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString InString;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool PrintToScreen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool PrintToLog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FColor TextColor;
public:
	UPrintStringNode();

	virtual EPerformNodeResult PerformNode() override;
	
	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys);  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API  UAddScreenMessageNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Duration;

public:
	UAddScreenMessageNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UAddTargetObjectToPhaseNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()
public:
	UAddTargetObjectToPhaseNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;
};


UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UMessageStartNode : public UDialogStartNode
{
	GENERATED_BODY()

public:
	UMessageStartNode();


};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UMessageNode : public UDialogNode
{
	GENERATED_BODY()

public:
	UMessageNode();


};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UMessageEndNode : public UDialogEndNode
{
	GENERATED_BODY()

public:
	UMessageEndNode();


};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UMessageExitNode : public UDialogExitNode
{
	GENERATED_BODY()

public:
	UMessageExitNode();


};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UActivateTriggerNode : public UDialogNodeBase
{
	GENERATED_BODY()

public:
	UActivateTriggerNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;


};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API USetInventoryItemStateNode : public UDialogNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int NewCurentInventoryItemState;

public:
	USetInventoryItemStateNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

	void SetCurrentState(int State);

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys) override;  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API USetInventoryItemStateFromMessageNode : public USetInventoryItemStateNode
{
	GENERATED_BODY()
public:
	USetInventoryItemStateFromMessageNode();
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UPrintQuestPhaseOnScreenNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()


public:
	UPrintQuestPhaseOnScreenNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API UAddMessageBranchNode : public UAddDialogNode
{
	GENERATED_BODY()


public:
	UAddMessageBranchNode();

	
};

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API USendMessagToLevelBlueprintNode : public UStoryHorizontalNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Message;
public:
	USendMessagToLevelBlueprintNode();

	virtual EPerformNodeResult PerformNode() override;

	virtual FText GetNodeTitle() const override;

#if WITH_EDITORONLY_DATA
	virtual void GetXMLSavingProperty(std::map<FString, XMLProperty>& Propertys);  

	virtual void LoadPropertyFromXML(std::map<FString, XMLProperty>& Propertys) override;
#endif //WITH_EDITORONLY_DATA

};